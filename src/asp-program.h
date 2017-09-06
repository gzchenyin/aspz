/*
 * asp-program.h
 *
 *  Created on: July 15, 2016
 *      Author: ychen
 */

#ifndef ASP_PROGRAM_H_
#define ASP_PROGRAM_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "mtl/Vec.h"
#include "minisat-SolverTypes.h"
#include "minisat-Solver.h"

using namespace Minisat;
using namespace std;

namespace aspz{
//supported rule type from lparse
enum RuleType {
    ENDRULE             = 0,
    BASICRULE           = 1,
    DISJUNCTIVERULE     = 8
};

class Atom
{
public:
    Atom(){val = l_Undef;};
    string name;
    lbool val;  //truth value of the atom
    vector<int> hrules; //the head of the rules
    //vector<int> prules; //the positive part of the rules
    
    void dump();
};

class Rule
{
public:
    Rule(){};
    vector<Var> head;
    vector<Var> pos;
    vector<Var> neg;
    
    void dump();
};

class AspProgram
{
public:
    AspProgram();
    ~AspProgram();
    vector<Atom> atoms;
    vector<Rule> rules;
    Solver sat_solver;
    
//output program for debug    
    void dump();
    
//read program
    bool read(ifstream &i);
    void new_atom(int ano);
    int new_tmp_atom();
    void add_rule(Rule &r);
    bool read_lparse(ifstream &i);
    bool read_normal_rule(ifstream &i);
    bool read_atoms(ifstream& i, vector<Var> &v, int n);
    
//solve the program
    bool sat_init();
    bool sat_add_if();
    bool sat_add_onlyif();
    
    lbool asp_solve();
    CRef sat_propagate(); 
    CRef asp_propagate(); 
    void learn_from_sat(CRef confl);
    void learn_from_asp(CRef confl);
    lbool asp_check();
    
    
    void get_model();
    //lbool asp_propagate(vec<Lit>& lpar, vec<Lit>& conflict)
    //Lit asp_desion(vec<Lit>& lpar)
    //lbool asp_check(
    
    lbool search(int nof_conflicts);
    
    void interrupt(){};
    int sat_restart_para(int curr_restarts);
};

}
#endif /* ASP_PROGRAM_H_ */
