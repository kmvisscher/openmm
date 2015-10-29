/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2010 Stanford University and the Authors.           *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

#include "openmm/serialization/SystemProxy.h"
#include "openmm/serialization/SerializationNode.h"
#include "openmm/Force.h"
#include "openmm/System.h"
#include "openmm/VirtualSite.h"
#include <sstream>

using namespace OpenMM;
using namespace std;

SystemProxy::SystemProxy() : SerializationProxy("System") {
}

void SystemProxy::serialize(const void* object, SerializationNode& node) const {
    node.setIntProperty("version", 1);
    const System& system = *reinterpret_cast<const System*>(object);
    Vec3 a, b, c;
    system.getDefaultPeriodicBoxVectors(a, b, c);
    SerializationNode& box = node.createChildNode("PeriodicBoxVectors");
    box.createChildNode("A").setDoubleProperty("x", a[0]).setDoubleProperty("y", a[1]).setDoubleProperty("z", a[2]);
    box.createChildNode("B").setDoubleProperty("x", b[0]).setDoubleProperty("y", b[1]).setDoubleProperty("z", b[2]);
    box.createChildNode("C").setDoubleProperty("x", c[0]).setDoubleProperty("y", c[1]).setDoubleProperty("z", c[2]);
    SerializationNode& particles = node.createChildNode("Particles");
    for (int i = 0; i < system.getNumParticles(); i++) {
        SerializationNode& particle = particles.createChildNode("Particle").setDoubleProperty("mass", system.getParticleMass(i));
        if (system.isVirtualSite(i)) {
            if (typeid(system.getVirtualSite(i)) == typeid(TwoParticleAverageSite)) {
                const TwoParticleAverageSite& site = dynamic_cast<const TwoParticleAverageSite&>(system.getVirtualSite(i));
                particle.createChildNode("TwoParticleAverageSite").setIntProperty("p1", site.getParticle(0)).setIntProperty("p2", site.getParticle(1)).setDoubleProperty("w1", site.getWeight(0)).setDoubleProperty("w2", site.getWeight(1));
            }
            else if (typeid(system.getVirtualSite(i)) == typeid(ThreeParticleAverageSite)) {
                const ThreeParticleAverageSite& site = dynamic_cast<const ThreeParticleAverageSite&>(system.getVirtualSite(i));
                particle.createChildNode("ThreeParticleAverageSite").setIntProperty("p1", site.getParticle(0)).setIntProperty("p2", site.getParticle(1)).setIntProperty("p3", site.getParticle(2)).setDoubleProperty("w1", site.getWeight(0)).setDoubleProperty("w2", site.getWeight(1)).setDoubleProperty("w3", site.getWeight(2));
            }
            else if (typeid(system.getVirtualSite(i)) == typeid(OutOfPlaneSite)) {
                const OutOfPlaneSite& site = dynamic_cast<const OutOfPlaneSite&>(system.getVirtualSite(i));
                particle.createChildNode("OutOfPlaneSite").setIntProperty("p1", site.getParticle(0)).setIntProperty("p2", site.getParticle(1)).setIntProperty("p3", site.getParticle(2)).setDoubleProperty("w12", site.getWeight12()).setDoubleProperty("w13", site.getWeight13()).setDoubleProperty("wc", site.getWeightCross());
            }
            else if (typeid(system.getVirtualSite(i)) == typeid(LocalCoordinatesSite)) {
                const LocalCoordinatesSite& site = dynamic_cast<const LocalCoordinatesSite&>(system.getVirtualSite(i));
                Vec3 wo = site.getOriginWeights();
                Vec3 wx = site.getXWeights();
                Vec3 wy = site.getYWeights();
                Vec3 p = site.getLocalPosition();
                particle.createChildNode("LocalCoordinatesSite").setIntProperty("p1", site.getParticle(0)).setIntProperty("p2", site.getParticle(1)).setIntProperty("p3", site.getParticle(2)).
                        setDoubleProperty("wo1", wo[0]).setDoubleProperty("wo2", wo[1]).setDoubleProperty("wo3", wo[2]).
                        setDoubleProperty("wx1", wx[0]).setDoubleProperty("wx2", wx[1]).setDoubleProperty("wx3", wx[2]).
                        setDoubleProperty("wy1", wy[0]).setDoubleProperty("wy2", wy[1]).setDoubleProperty("wy3", wy[2]).
                        setDoubleProperty("pos1", p[0]).setDoubleProperty("pos2", p[1]).setDoubleProperty("pos3", p[2]);
            }
	    else if (typeid(system.getVirtualSite(i)) == typeid(ParticleGroupAverageSite)) {
                const ParticleGroupAverageSite& site = dynamic_cast<const ParticleGroupAverageSite&>(system.getVirtualSite(i));
                particle.createChildNode("ParticleGroupAverageSite").setIntProperty("p1", site.getParticle(0)).setIntProperty("p2", site.getParticle(1)).setIntProperty("p3", site.getParticle(2)).setIntProperty("p4", site.getParticle(3)).setIntProperty("p5", site.getParticle(4)).setIntProperty("p6", site.getParticle(5)).setIntProperty("p7", site.getParticle(6)).setIntProperty("p8", site.getParticle(7)).setIntProperty("p9", site.getParticle(8)).setIntProperty("p10", site.getParticle(9)).setIntProperty("p11", site.getParticle(10)).setIntProperty("p12", site.getParticle(11)).setIntProperty("p13", site.getParticle(12)).setIntProperty("p14", site.getParticle(13)).setIntProperty("p15", site.getParticle(14)).setDoubleProperty("w1", site.getWeight(0)).setDoubleProperty("w2", site.getWeight(1)).setDoubleProperty("w3", site.getWeight(2)).setDoubleProperty("w4", site.getWeight(3)).setDoubleProperty("w5", site.getWeight(4)).setDoubleProperty("w6", site.getWeight(5)).setDoubleProperty("w7", site.getWeight(6)).setDoubleProperty("w8", site.getWeight(7)).setDoubleProperty("w9", site.getWeight(8)).setDoubleProperty("w10", site.getWeight(9)).setDoubleProperty("w11", site.getWeight(10)).setDoubleProperty("w12", site.getWeight(11)).setDoubleProperty("w13", site.getWeight(12)).setDoubleProperty("w14", site.getWeight(13)).setDoubleProperty("w15", site.getWeight(14));
            }
	    {	

	    }
        }
    }
    SerializationNode& constraints = node.createChildNode("Constraints");
    for (int i = 0; i < system.getNumConstraints(); i++) {
        int particle1, particle2;
        double distance;
        system.getConstraintParameters(i, particle1, particle2, distance);
        constraints.createChildNode("Constraint").setIntProperty("p1", particle1).setIntProperty("p2", particle2).setDoubleProperty("d", distance);
    }
    SerializationNode& forces = node.createChildNode("Forces");
    for (int i = 0; i < system.getNumForces(); i++)
        forces.createChildNode("Force", &system.getForce(i));
}

void* SystemProxy::deserialize(const SerializationNode& node) const {
    if (node.getIntProperty("version") != 1)
        throw OpenMMException("Unsupported version number");
    System* system = new System();
    try {
        const SerializationNode& box = node.getChildNode("PeriodicBoxVectors");
        const SerializationNode& boxa = box.getChildNode("A");
        const SerializationNode& boxb = box.getChildNode("B");
        const SerializationNode& boxc = box.getChildNode("C");
        Vec3 a(boxa.getDoubleProperty("x"), boxa.getDoubleProperty("y"), boxa.getDoubleProperty("z"));
        Vec3 b(boxb.getDoubleProperty("x"), boxb.getDoubleProperty("y"), boxb.getDoubleProperty("z"));
        Vec3 c(boxc.getDoubleProperty("x"), boxc.getDoubleProperty("y"), boxc.getDoubleProperty("z"));
        system->setDefaultPeriodicBoxVectors(a, b, c);
        const SerializationNode& particles = node.getChildNode("Particles");
        for (int i = 0; i < (int) particles.getChildren().size(); i++) {
            system->addParticle(particles.getChildren()[i].getDoubleProperty("mass"));
            if (particles.getChildren()[i].getChildren().size() > 0) {
                const SerializationNode& vsite = particles.getChildren()[i].getChildren()[0];
                if (vsite.getName() == "TwoParticleAverageSite")
                    system->setVirtualSite(i, new TwoParticleAverageSite(vsite.getIntProperty("p1"), vsite.getIntProperty("p2"), vsite.getDoubleProperty("w1"), vsite.getDoubleProperty("w2")));
                else if (vsite.getName() == "ThreeParticleAverageSite")
                    system->setVirtualSite(i, new ThreeParticleAverageSite(vsite.getIntProperty("p1"), vsite.getIntProperty("p2"), vsite.getIntProperty("p3"), vsite.getDoubleProperty("w1"), vsite.getDoubleProperty("w2"), vsite.getDoubleProperty("w3")));
                else if (vsite.getName() == "OutOfPlaneSite")
                    system->setVirtualSite(i, new OutOfPlaneSite(vsite.getIntProperty("p1"), vsite.getIntProperty("p2"), vsite.getIntProperty("p3"), vsite.getDoubleProperty("w12"), vsite.getDoubleProperty("w13"), vsite.getDoubleProperty("wc")));
                else if (vsite.getName() == "LocalCoordinatesSite") {
                    Vec3 wo(vsite.getDoubleProperty("wo1"), vsite.getDoubleProperty("wo2"), vsite.getDoubleProperty("wo3"));
                    Vec3 wx(vsite.getDoubleProperty("wx1"), vsite.getDoubleProperty("wx2"), vsite.getDoubleProperty("wx3"));
                    Vec3 wy(vsite.getDoubleProperty("wy1"), vsite.getDoubleProperty("wy2"), vsite.getDoubleProperty("wy3"));
                    Vec3 p(vsite.getDoubleProperty("pos1"), vsite.getDoubleProperty("pos2"), vsite.getDoubleProperty("pos3"));
                    system->setVirtualSite(i, new LocalCoordinatesSite(vsite.getIntProperty("p1"), vsite.getIntProperty("p2"), vsite.getIntProperty("p3"), wo, wx, wy, p));
                }
	        else if (vsite.getName() == "ParticleGroupAverageSite")
		{
		    std::vector< int > indices;
		    std::vector< double > weights;
		
		    indices.push_back(vsite.getIntProperty("p1"));
                    indices.push_back(vsite.getIntProperty("p2"));
                    indices.push_back(vsite.getIntProperty("p3"));
                    indices.push_back(vsite.getIntProperty("p4"));
                    indices.push_back(vsite.getIntProperty("p5"));
                    indices.push_back(vsite.getIntProperty("p6"));
                    indices.push_back(vsite.getIntProperty("p7"));
                    indices.push_back(vsite.getIntProperty("p8"));
                    indices.push_back(vsite.getIntProperty("p9"));
                    indices.push_back(vsite.getIntProperty("p10"));
                    indices.push_back(vsite.getIntProperty("p11"));
                    indices.push_back(vsite.getIntProperty("p12"));
                    indices.push_back(vsite.getIntProperty("p13"));
                    indices.push_back(vsite.getIntProperty("p14"));
                    indices.push_back(vsite.getIntProperty("p15"));
                    weights.push_back(vsite.getDoubleProperty("w1"));
                    weights.push_back(vsite.getDoubleProperty("w2"));
                    weights.push_back(vsite.getDoubleProperty("w3"));
                    weights.push_back(vsite.getDoubleProperty("w4"));
                    weights.push_back(vsite.getDoubleProperty("w5"));
                    weights.push_back(vsite.getDoubleProperty("w6"));
                    weights.push_back(vsite.getDoubleProperty("w7"));
                    weights.push_back(vsite.getDoubleProperty("w8"));
                    weights.push_back(vsite.getDoubleProperty("w9"));
                    weights.push_back(vsite.getDoubleProperty("w10"));
                    weights.push_back(vsite.getDoubleProperty("w11"));
                    weights.push_back(vsite.getDoubleProperty("w12"));
                    weights.push_back(vsite.getDoubleProperty("w13"));
                    weights.push_back(vsite.getDoubleProperty("w14"));
                    weights.push_back(vsite.getDoubleProperty("w15"));

	        }
            }
        }
        const SerializationNode& constraints = node.getChildNode("Constraints");
        for (int i = 0; i < (int) constraints.getChildren().size(); i++) {
            const SerializationNode& constraint = constraints.getChildren()[i];
            system->addConstraint(constraint.getIntProperty("p1"), constraint.getIntProperty("p2"), constraint.getDoubleProperty("d"));
        }
        const SerializationNode& forces = node.getChildNode("Forces");
        for (int i = 0; i < (int) forces.getChildren().size(); i++) {
            system->addForce(forces.getChildren()[i].decodeObject<Force>());
        }
    }
    catch (...) {
        delete system;
        throw;
    }
    return system;
}
