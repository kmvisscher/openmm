#ifndef OPENMM_KERNEL_H_
#define OPENMM_KERNEL_H_

/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2008 Stanford University and the Authors.           *
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

#include "KernelImpl.h"

namespace OpenMM {

/**
 * A Kernel encapsulates a particular implementation of a calculation that can be performed on Streams.
 * Kernel objects are created by Platforms:
 * 
 * <pre>
 * Kernel kernel = platform.createKernel(kernelName);
 * </pre>
 * 
 * The Kernel class itself does not specify any details of what calculation is to be performed or the API
 * for calling it.  Instead, subclasses of KernelImpl will define APIs which are appropriate to particular
 * calculations.  To execute a Kernel, you therefore requests its implementation object and cast it to the
 * correct type:
 * 
 * <pre>
 * dynamic_cast<AddStreamsImpl&>(kernel.getImpl()).execute(stream1, stream2);
 * </pre>
 */

class Kernel {
public:
    Kernel();
    Kernel(const Kernel& copy);
    ~Kernel();
    Kernel operator=(const Kernel& copy);
    /**
     * Get the name of this Kernel.
     */
    std::string getName() const;
    /**
     * Get the object which implements this Kernel.
     */
    KernelImpl& getImpl();
private:
    friend class Platform;
    /**
     * Create a KernelImpl.
     * 
     * @param name the name of the kernel to create
     */
    Kernel(KernelImpl* impl);
    KernelImpl* impl;
};

} // namespace OpenMM

#endif /*OPENMM_KERNEL_H_*/
