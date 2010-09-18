
/* Portions copyright (c) 2006 Stanford University and Simbios.
 * Contributors: Pande Group
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "AmoebaReferenceForce.h"
#include "AmoebaReferencePiTorsionForce.h"
#include <vector>

/**---------------------------------------------------------------------------------------

   Calculate Amoeba pi-torsion ixn (force and energy)

   @param positionAtomA           Cartesian coordinates of atom A
   @param positionAtomB           Cartesian coordinates of atom B
   @param positionAtomC           Cartesian coordinates of atom C
   @param positionAtomD           Cartesian coordinates of atom D
   @param positionAtomE           Cartesian coordinates of atom E
   @param positionAtomF           Cartesian coordinates of atom F
   @param piTorsionK              force constant
   @param forces                  force vector

   @return energy

   --------------------------------------------------------------------------------------- */

RealOpenMM AmoebaReferencePiTorsionForce::calculateForceAndEnergy( const RealOpenMM* positionAtomA, const RealOpenMM* positionAtomB,
                                                                   const RealOpenMM* positionAtomC, const RealOpenMM* positionAtomD,
                                                                   const RealOpenMM* positionAtomE, const RealOpenMM* positionAtomF,
                                                                   RealOpenMM piTorsionK, RealOpenMM** forces ){

   // ---------------------------------------------------------------------------------------

   //static const std::string methodName = "AmoebaReferencePiTorsionForce::calculateHarmonicForce";

   static const RealOpenMM zero          = 0.0;
   static const RealOpenMM one           = 1.0;
   static const RealOpenMM two           = 2.0;

   // ---------------------------------------------------------------------------------------

    enum { AD, BD, EC, FC, P, Q, CP, DC, QD, T, U, TU, DP, QC, dT, dU, dP, dQ, dC1, dC2, dD1, dD2, LastDeltaIndex };
 
    std::vector<RealOpenMM> deltaR[LastDeltaIndex];
    for( unsigned int ii = 0; ii < LastDeltaIndex; ii++ ){
        deltaR[ii].resize(3);
    }   
    AmoebaReferenceForce::loadDeltaR( positionAtomD, positionAtomA, deltaR[AD] );
    AmoebaReferenceForce::loadDeltaR( positionAtomD, positionAtomB, deltaR[BD] );
    AmoebaReferenceForce::loadDeltaR( positionAtomC, positionAtomE, deltaR[EC] );
    AmoebaReferenceForce::loadDeltaR( positionAtomC, positionAtomF, deltaR[FC] );

    enum { A, B, C, D, E, F, LastAtomIndex };
    std::vector<RealOpenMM> d[LastAtomIndex];
    for( unsigned int ii = 0; ii < LastAtomIndex; ii++ ){
        d[ii].resize(3);
    }   
 
    AmoebaReferenceForce::getCrossProduct( deltaR[AD], deltaR[BD], deltaR[P] );
    AmoebaReferenceForce::getCrossProduct( deltaR[EC], deltaR[FC], deltaR[Q] );
    for( int ii = 0; ii < 3; ii++ ){
       deltaR[CP][ii]  = -deltaR[P][ii];
       deltaR[DC][ii]  =  positionAtomD[ii] - positionAtomC[ii];
       deltaR[QD][ii]  =  deltaR[Q][ii];
 
       deltaR[P][ii]  += positionAtomC[ii];
       deltaR[Q][ii]  += positionAtomD[ii];
    }
    AmoebaReferenceForce::getCrossProduct( deltaR[CP], deltaR[DC], deltaR[T]  );
    AmoebaReferenceForce::getCrossProduct( deltaR[DC], deltaR[QD], deltaR[U]  );
    AmoebaReferenceForce::getCrossProduct( deltaR[T],  deltaR[U],  deltaR[TU] );
 
    RealOpenMM rT2  = AmoebaReferenceForce::getNormSquared3( deltaR[T] );
    RealOpenMM rU2  = AmoebaReferenceForce::getNormSquared3( deltaR[U] );
    RealOpenMM rTrU = SQRT( rT2*rU2 );
    if( rTrU <= zero ){
       return zero;
    }
 
    RealOpenMM rDC     = AmoebaReferenceForce::getNorm3( deltaR[DC] );
  
    RealOpenMM cosine  = AmoebaReferenceForce::getDotProduct3( deltaR[T], deltaR[U] );
               cosine /= rTrU;
 
    RealOpenMM sine    = AmoebaReferenceForce::getDotProduct3( deltaR[DC], deltaR[TU] );
                sine  /= ( rDC*rTrU );
 
    RealOpenMM cosine2 = cosine*cosine - sine*sine;
    RealOpenMM sine2   = two*cosine*sine;
 
    RealOpenMM phi2    = one - cosine2;
    RealOpenMM dphi2   = two*sine2;
 
    RealOpenMM dedphi  = piTorsionK*dphi2; 
 
    for( unsigned int ii = 0; ii < 3; ii++ ){
       deltaR[DP][ii] = positionAtomD[ii] - deltaR[P][ii];
       deltaR[QC][ii] = deltaR[Q][ii]     - positionAtomC[ii];
    }
 
    RealOpenMM factorT =  dedphi/( rDC*rT2 );
    RealOpenMM factorU = -dedphi/( rDC*rU2 );
 
    AmoebaReferenceForce::getCrossProduct( deltaR[T], deltaR[DC], deltaR[dT]  );
    AmoebaReferenceForce::getCrossProduct( deltaR[U], deltaR[DC], deltaR[dU]  );
    for( int ii = 0; ii < 3; ii++ ){
       deltaR[dT][ii] *= factorT;
       deltaR[dU][ii] *= factorU;
    }
 
    AmoebaReferenceForce::getCrossProduct( deltaR[dT], deltaR[DC], deltaR[dP]   );
    AmoebaReferenceForce::getCrossProduct( deltaR[dU], deltaR[DC], deltaR[dQ]   );
 
    AmoebaReferenceForce::getCrossProduct( deltaR[DP], deltaR[dT], deltaR[dC1]  );
    AmoebaReferenceForce::getCrossProduct( deltaR[dU], deltaR[QD], deltaR[dC2]  );
 
    AmoebaReferenceForce::getCrossProduct( deltaR[dT], deltaR[CP], deltaR[dD1]  );
    AmoebaReferenceForce::getCrossProduct( deltaR[QC], deltaR[dU], deltaR[dD2]  );
 
    AmoebaReferenceForce::getCrossProduct( deltaR[BD], deltaR[dP], d[A]         );
    AmoebaReferenceForce::getCrossProduct( deltaR[dP], deltaR[AD], d[B]         );
 
    AmoebaReferenceForce::getCrossProduct( deltaR[FC], deltaR[dQ], d[E]         );
    AmoebaReferenceForce::getCrossProduct( deltaR[dQ], deltaR[EC], d[F]         );
 
    for( int ii = 0; ii < 3; ii++ ){
       d[C][ii] = deltaR[dC1][ii] + deltaR[dC2][ii] + deltaR[dP][ii] - d[E][ii] - d[F][ii];
       d[D][ii] = deltaR[dD1][ii] + deltaR[dD2][ii] + deltaR[dQ][ii] - d[A][ii] - d[B][ii];
    }
 
    // ---------------------------------------------------------------------------------------
 
    // add in forces
 
    forces[0][0] -= d[0][0];
    forces[0][1] -= d[0][1];
    forces[0][2] -= d[0][2];
 
    forces[1][0] -= d[1][0];
    forces[1][1] -= d[1][1];
    forces[1][2] -= d[1][2];
 
    forces[2][0] -= d[2][0];
    forces[2][1] -= d[2][1];
    forces[2][2] -= d[2][2];
 
    forces[3][0] -= d[3][0];
    forces[3][1] -= d[3][1];
    forces[3][2] -= d[3][2];
 
    forces[4][0] -= d[4][0];
    forces[4][1] -= d[4][1];
    forces[4][2] -= d[4][2];
 
    forces[5][0] -= d[5][0];
    forces[5][1] -= d[5][1];
    forces[5][2] -= d[5][2];
 
    // ---------------------------------------------------------------------------------------
 
    // calculate energy if 'energy' is set
 
    return piTorsionK*phi2; 
 
}