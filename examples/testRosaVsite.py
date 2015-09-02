from simtk.openmm.app import *
from simtk.openmm import *
from simtk.unit import *
from sys import stdout

from itertools import repeat


system = System();
#add a series of particles
for i in range( 0, 15 ):
    system.addParticle(1.0);

vsiteAtoms = [];
vsiteWeights = [];

for i in range( 0, 15 ):
    vsiteAtoms.append( i );
    vsiteWeights.append( ( 1.0 + i ) / 120.0 );

# New vsite definition
system.addParticle(0.0);
system.setVirtualSite(15, ParticleGroupAverageSite(vsiteAtoms, vsiteWeights) );

forceField = CustomExternalForce("-a*x");
system.addForce(forceField);
forceField.addPerParticleParameter("a");

params = [ 0.0 ];

for i in range( 0, 16 ):
    params[0] = 0.1 * i + 0.1;
    forceField.addParticle(i, params);


integrator = LangevinIntegrator(300*kelvin, 0.1/picosecond, 0.002*picoseconds)
platform = Platform.getPlatformByName("OpenCL");
context = Context(system, integrator, platform);

positions = list(repeat( Vec3(0.0,0.0,0.0), 16));

for i in range( 0, 5 ):
    positions[i*3]   = Vec3(i*3, 0, 0);
    positions[i*3+1] = Vec3(0, i*3+1, 0);
    positions[i*3+2] = Vec3(0, 0, i*3+2);
	
context.setPositions(positions);
context.applyConstraints(0.0001);

for i in range( 0, 1000 ):
    state = context.getState( 1 + 4 );
    pos = state.getPositions();
    
    #calculate the average position
    refPos = Vec3(  Quantity( 0.0, nanometer ),
                    Quantity( 0.0, nanometer ),
                    Quantity( 0.0, nanometer ) );

    for subIndex in range( 0, 15 ):
       
        localWeight = ( 1.0  + subIndex ) / 120.0;
        #print ("%i %i" % (is_quantity( pos[subIndex] ), is_quantity( Quantity( localWeight, nanometer ) ) ) );
   
        refPos += pos[subIndex] * Quantity( localWeight );
    
    print "Iterator: "
    print "\t Ref: ", refPos, "\n", "\t Act: ", pos[15];

    integrator.step(1);