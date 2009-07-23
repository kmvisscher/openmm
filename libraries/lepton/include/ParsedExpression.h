#ifndef LEPTON_PARSED_EXPRESSION_H_
#define LEPTON_PARSED_EXPRESSION_H_

/* -------------------------------------------------------------------------- *
 *                                   Lepton                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the Lepton expression parser originating from              *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2009 Stanford University and the Authors.           *
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

#include "ExpressionTreeNode.h"
#include "windowsIncludes.h"
#include <map>
#include <string>

namespace Lepton {

/**
 * This class represents the result of parsing an expression.  It provides methods for working with the
 * expression in various ways, such as evaluating it, getting the tree representation of the expresson, etc.
 */

class LEPTON_EXPORT ParsedExpression {
public:
    /**
     * Create a ParsedExpression.  Normally you will not call this directly.  Instead, use the Parser class
     * to parse expression.
     */
    ParsedExpression(ExpressionTreeNode rootNode);
    /**
     * Get the root node of the expression's abstract syntax tree.
     */
    const ExpressionTreeNode& getRootNode() const;
    /**
     * Evaluate the expression.  If the expression involves any variables, this method will throw an exception.
     */
    double evaluate() const;
    /**
     * Evaluate the expression.
     *
     * @param variables    a map specifying the values of all variables that appear in the expression.  If any
     *                     variable appears in the expression but is not included in this map, an exception
     *                     will be thrown.
     */
    double evaluate(const std::map<std::string, double>& variables) const;
    /**
     * Create a new ParsedExpression which produces the same result as this one, but is faster to evaluate.
     */
    ParsedExpression optimize() const;
    /**
     * Create a new ParsedExpression which produces the same result as this one, but is faster to evaluate.
     *
     * @param variables    a map specifying values for a subset of variables that appear in the expression.
     *                     All occurrences of these variables in the expression are replaced with the values
     *                     specified.
     */
    ParsedExpression optimize(const std::map<std::string, double>& variables) const;
private:
    static double evaluate(const ExpressionTreeNode& node, const std::map<std::string, double>& variables);
    static ExpressionTreeNode preevaluateVariables(const ExpressionTreeNode& node, const std::map<std::string, double>& variables);
    static ExpressionTreeNode precalculateConstantSubexpressions(const ExpressionTreeNode& node);
    static ExpressionTreeNode substituteSimplerExpression(const ExpressionTreeNode& node);
    ExpressionTreeNode rootNode;
};

} // namespace Lepton

#endif /*LEPTON_PARSED_EXPRESSION_H_*/