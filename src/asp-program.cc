/*
 * asp-program.cc
 *
 *  Created on: July 15, 2016
 *      Author: ychen
 */

#include <signal.h>

#include "utils/Options.h"
#include "utils/System.h"
#include "mtl/Vec.h"

#include "asp-program.h"

using namespace Minisat;
using namespace std;
using namespace aspz;


void printVec(vec<Lit>& l)
{
  for (int i = 0; i < l.size(); i++)
    cout << l[i].x << " ";
  cout << endl;
}

void Atom::dump()
{
  cout << name;
  if (val == l_True)
    cout << " (true)";
  else if (val == l_False)
    cout << " (false)";
  else
    cout << " (undef)";
  cout << endl;
  
  cout << "  rules: ";
  for (vector<int>::iterator it = hrules.begin(); it != hrules.end(); it++)
    cout << *it << " ";
  cout << endl;
}

void Rule::dump()
{
  cout << "(";
  for (vector<Var>::iterator it = head.begin(); it != head.end(); it++)
    cout << *it << " ";
  cout << ")";
  
  cout << ":- ";

  cout << "(";
  for (vector<Var>::iterator it = pos.begin(); it != pos.end(); it++)
    cout << *it << " ";
  cout << ")";

  cout << "not ";
  
  cout << "(";
  for (vector<Var>::iterator it = neg.begin(); it != neg.end(); it++)
    cout << *it << " ";
  cout << ")";
  
  cout << endl;
}

AspProgram::AspProgram()
{
  Atom a;
  a.name = "_empty";
  atoms.push_back(a);
  
  a.name = "_false";
  atoms.push_back(a);
}

AspProgram::~AspProgram()
{
}

bool AspProgram::read(ifstream& i)
{
  return read_lparse(i);
}

bool AspProgram::read_lparse(ifstream& i)
{
    // Read rules.
    int type;
    int lineno = 0;
    bool ret = true;

    i >> type;
    while (ret && (type != ENDRULE))
    {
        // Rule Type
        switch (type)
        {
        case BASICRULE:
            ret = read_normal_rule(i);
            break;
        case DISJUNCTIVERULE:
        default:
            cout << "unsupported rule type on line " << lineno << endl;
            ret = false;
        }
        lineno++;
        i >> type;
    }

    // Read atom names
    string s;
    int ano;
    getline(i, s); //expected atom names to begin on new line
    i >> ano;
    getline(i, s);
    lineno++;
    while (ano)
    {
        atoms[ano].name = s;
        i >> ano;
        getline(i, s);
        lineno++;
    }
/*
    //unsolved atom num
    for (atom_idx i = 1; i <= g_dlp.max_atom; i++)
    {
        if (g_dlp.atoms[i].unsolved)
	  g_dlp.ua_num++;
    }
    cout << endl;
*/
    return ret;

    /*
        // Read compute-statement
        f.getline (s, len);
        if (!f.good () || strcmp (s, "B+"))
        {
                cerr << "B+ expected, line " << linenumber << endl;
                return 1;
        }
        linenumber++;
        f >> i;
        linenumber++;
        while (i)
        {
                if (!f.good () || i < 1)
                {
                        cerr << "B+ atom out of bounds, line " << linenumber << endl;
                        return 1;
                }
                Atom *a = getAtom (i);
                api->set_compute (a, true);
                f >> i;
                linenumber++;
        }
        f.getline (s, len);  // Get newline.
        f.getline (s, len);
        if (!f.good () || strcmp (s, "B-"))
        {
                cerr << "B- expected, line " << linenumber << endl;
                return 1;
        }
        linenumber++;
        f >> i;
        linenumber++;
        while (i)
        {
                if (!f.good () || i < 1)
                {
                        cerr << "B- atom out of bounds, line " << linenumber << endl;
                        return 1;
                }
                Atom *a = getAtom (i);
                api->set_compute (a, false);
                f >> i;
                linenumber++;
        }
        f >> models;  // zero means all
        if (f.fail ())
        {
                cerr << "number of models expected, line " << linenumber << endl;
                return 1;
        }
        return 0;*/
}

void AspProgram::add_rule(Rule& r)
{
  if ((r.head.size() == 1) && (r.pos.size() == 0) && (r.neg.size() == 0)) //fact
    atoms[r.head[0]].val = l_True;
  else //normal rule
  {
    //add reference to the atom of the head
    atoms[r.head[0]].hrules.push_back(rules.size());
    //add rule
    rules.push_back(r);
  }
}

bool AspProgram::read_normal_rule(ifstream& i)
{
    Rule r;
    Var n;
    bool ret = true;
    int body, neg;

    //head
    ret = read_atoms(i, r.head, 1);

    // Body
    if (ret)
    {
      i >> n;
    if (!i.good () || n < 0)
    {
        cout << "body atom out of bounds" << endl;
        ret = false;
    }
    }
    
    if (ret)
    {
      body = n;
    // Negative body
    i >> n;
    if (!i.good () || n < 0 || n > body)
    {
        cout << "neg atom out of bounds" << endl;
        ret = false;
    }
    neg = n;
    }

    if (ret)
    ret = read_atoms(i, r.neg, neg);
    
    if (ret)
     ret = read_atoms(i, r.pos, body - neg);

    add_rule(r);

    return ret;

}

bool AspProgram::read_atoms(ifstream& i, vector<Var> &v, int n)
{
    int j;
    bool ret = true;
    
    for (int k = 0; (k < n) && ret; k++)
    {
        i >> j;
	if (!i.good())
	  ret = false;
        new_atom(j);
        v.push_back(j);
    }
    
    return ret;
}

void AspProgram::new_atom(int ano)
{
    Atom a;
    
    int i = atoms.size();
    
    while ( ano >= i )
    {
        i++;
        atoms.push_back( a );
    }
}


void AspProgram::dump()
{
  int i = 0;
  
  cout << "atoms(" << atoms.size() << "):" << endl;
  for (vector<Atom>::iterator it = atoms.begin(); it != atoms.end(); it++)
  {
    cout << "[" << i++ << "] ";
    it->dump();
  }
  
  i = 0;
  cout << "rules(" << rules.size() << "): " << endl;
  for (vector<Rule>::iterator it = rules.begin(); it != rules.end(); it++)
  {
    cout << "[" << i++ << "] ";
    it->dump();
  }
  
}

bool AspProgram::sat_init()
{  
  for (size_t i = 0; i < atoms.size(); i++)
    sat_solver.newVar();
  
  bool ret = sat_add_if();
  
  if (ret)
    sat_add_onlyif();
  
  if (ret)
  {
    sat_solver.model.clear();
    sat_solver.conflict.clear();

    sat_solver.solves++;

    sat_solver.max_learnts               = sat_solver.nClauses() * sat_solver.learntsize_factor;
    sat_solver.learntsize_adjust_confl   = sat_solver.learntsize_adjust_start_confl;
    sat_solver.learntsize_adjust_cnt     = (int)sat_solver.learntsize_adjust_confl;
  }
  
  return ret;
}

bool AspProgram::sat_add_if()
{
  bool ret = true;
  
  for (vector<Rule>::iterator rit = rules.begin(); (ret && (rit != rules.end())); rit++)
  {
    vec<Lit> l;
    for (vector<Var>::iterator it = rit->head.begin(); it != rit->head.end(); it++)
    {
      //cout << "**" << *it-1 << " " << mkLit(*it-1).x << "  " << (~mkLit(*it-1)).x << endl;
      l.push(mkLit(*it-1));
    }
    for (vector<Var>::iterator it = rit->pos.begin(); it != rit->pos.end(); it++)
      l.push(~mkLit(*it-1));
    for (vector<Var>::iterator it = rit->neg.begin(); it != rit->neg.end(); it++)
      l.push(mkLit(*it-1));
    
    printVec(l);
    ret = sat_solver.addClause_(l);
  }

  return ret;
}

bool AspProgram::sat_add_onlyif()
{
  bool ret = true;
  
  return ret;
}

lbool AspProgram::asp_solve()
{
//Minisat::Solver::solve_()
  
  if (!sat_init())
    return l_False;
  
  if (!sat_solver.simplify())
    return l_False;
        
  lbool status = l_Undef;
  // Search:
  int curr_restarts = 0;
  while (status == l_Undef)
  {
    status = sat_solver.search(sat_restart_para(curr_restarts));
    if (!sat_solver.withinBudget()) break;
      curr_restarts++;
  }

  if (status == l_True)
  {
    // Extend & copy model:
    sat_solver.model.growTo(sat_solver.nVars());
    for (int i = 0; i < sat_solver.nVars(); i++)
      sat_solver.model[i] = sat_solver.value(i);
  }
  else if (status == l_False && sat_solver.conflict.size() == 0)
    sat_solver.ok = false;

  sat_solver.cancelUntil(0);
  return status;
}

void AspProgram::get_model()
{
  for (int i = 0; i < sat_solver.nVars(); i++)
  {
    atoms[i].val = sat_solver.model[i];
    if (sat_solver.model[i] != l_Undef)
    {
      if (i != 0)
	cout << " ";
      if (sat_solver.model[i]==l_False)
	cout << "-";
      cout << i+1;
    }
  }
  cout << endl;
}

/*
  Finite subsequences of the Luby-sequence:

  0: 1
  1: 1 1 2
  2: 1 1 2 1 1 2 4
  3: 1 1 2 1 1 2 4 1 1 2 1 1 2 4 8
  ...


 */

static double luby(double y, int x){

    // Find the finite subsequence that contains index 'x', and the
    // size of that subsequence:
    int size, seq;
    for (size = 1, seq = 0; size < x+1; seq++, size = 2*size+1);

    while (size-1 != x){
        size = (size-1)>>1;
        seq--;
        x = x % size;
    }

    return pow(y, seq);
}

int AspProgram::sat_restart_para(int curr_restarts)
{
  double rest_base = sat_solver.luby_restart ? 
    luby(sat_solver.restart_inc, curr_restarts) : 
    pow(sat_solver.restart_inc, curr_restarts);
    
  return rest_base * sat_solver.restart_first;
}



/*_________________________________________________________________________________________________
|
|  search : (nof_conflicts : int) (params : const SearchParams&)  ->  [lbool]
|  
|  Description:
|    Search for a model the specified number of conflicts. 
|    NOTE! Use negative value for 'nof_conflicts' indicate infinity.
|  
|  Output:
|    'l_True' if a partial assigment that is consistent with respect to the clauseset is found. If
|    all variables are decision variables, this means that the clause set is satisfiable. 'l_False'
|    if the clause set is unsatisfiable. 'l_Undef' if the bound on number of conflicts is reached.
|________________________________________________________________________________________________@*/
lbool AspProgram::search(int nof_conflicts)
{
  assert(sat_solver.ok);
  
  //int backtrack_level;
  int conflictC = 0;
  //vec<Lit> learnt_clause;
  sat_solver.starts++;
  
  for (;;)
  {
    CRef confl = sat_propagate();
    if (confl != CRef_Undef)
    {
      //CONFLICT FROM SAT SOLVER
      conflictC++;
      sat_solver.conflicts++;
      if (sat_solver.decisionLevel() == 0) 
        return l_False;
      
      learn_from_sat(confl);
    }
    else 
    {
      //NO CONFLICT FROM SAT SOLVER
      confl = asp_propagate();
      if (confl != CRef_Undef)
      {
	//CONFLICT FROM ASP SOLVER
        if (sat_solver.decisionLevel() == 0) 
          return l_False;
      
        learn_from_asp(confl);
      }
      else
      {
	//NO CONFLICT //UGLY!
        if (nof_conflicts >= 0 && conflictC >= nof_conflicts || !sat_solver.withinBudget()){
                // Reached bound on number of conflicts:
                sat_solver.progress_estimate = sat_solver.progressEstimate();
                sat_solver.cancelUntil(0);
                return l_Undef; }

            // Simplify the set of problem clauses:
            if (sat_solver.decisionLevel() == 0 && !sat_solver.simplify())
                return l_False;

            if (sat_solver.learnts.size()-sat_solver.nAssigns() >= sat_solver.max_learnts)
                // Reduce the set of learnt clauses:
                sat_solver.reduceDB();

            Lit next = lit_Undef;
            while (sat_solver.decisionLevel() < sat_solver.assumptions.size()){
                // Perform user provided assumption:
                Lit p = sat_solver.assumptions[sat_solver.decisionLevel()];
                if (sat_solver.value(p) == l_True){
                    // Dummy decision level:
                    sat_solver.newDecisionLevel();
                }else if (sat_solver.value(p) == l_False){
                    sat_solver.analyzeFinal(~p, sat_solver.conflict);
                    return l_False;
                }else{
                    next = p;
                    break;
                }
            }

            if (next == lit_Undef){
                // New variable decision:
                sat_solver.decisions++;
                next = sat_solver.pickBranchLit();

                if (next == lit_Undef)
		{
		  //SAT Model found
		  if (asp_check() == l_True)
		    //Answer set found
                    return l_True;
		}
		else
		{
                  // Increase decision level and enqueue 'next'
                  sat_solver.newDecisionLevel();
                  sat_solver.uncheckedEnqueue(next);
		}
            }
            

            // Increase decision level and enqueue 'next'
            sat_solver.newDecisionLevel();
            sat_solver.uncheckedEnqueue(next);
        }
      }
    }
}

CRef AspProgram::sat_propagate()
{
  return sat_solver.propagate();
}

CRef AspProgram::asp_propagate()
{
  return CRef_Undef;
}

void AspProgram::learn_from_sat(CRef confl)
{
  //UGLY: copy from search()
  int backtrack_level;
  vec<Lit> learnt_clause;
  
  learnt_clause.clear();
  sat_solver.analyze(confl, learnt_clause, backtrack_level);
  sat_solver.cancelUntil(backtrack_level);

  if (learnt_clause.size() == 1)
  {
    sat_solver.uncheckedEnqueue(learnt_clause[0]);
  }
  else
  {
    CRef cr = sat_solver.ca.alloc(learnt_clause, true);
    sat_solver.learnts.push(cr);
    sat_solver.attachClause(cr);
    sat_solver.claBumpActivity(sat_solver.ca[cr]);
    sat_solver.uncheckedEnqueue(learnt_clause[0], cr);
  }

  sat_solver.varDecayActivity();
  sat_solver.claDecayActivity();

  /*
  if (--learntsize_adjust_cnt == 0){
    learntsize_adjust_confl *= learntsize_adjust_inc;
    learntsize_adjust_cnt    = (int)learntsize_adjust_confl;
    max_learnts             *= learntsize_inc;

  if (verbosity >= 1)
    printf("| %9d | %7d %8d %8d | %8d %8d %6.0f | %6.3f %% |\n", 
      (int)conflicts, 
      (int)dec_vars - (trail_lim.size() == 0 ? trail.size() : trail_lim[0]), nClauses(), (int)clauses_literals, 
      (int)max_learnts, nLearnts(), (double)learnts_literals/nLearnts(), progressEstimate()*100);
  }
  */
}

void AspProgram::learn_from_asp(CRef confl)
{
  return;
}

lbool AspProgram::asp_check()
{
  return l_True;
  //if return, try to find a backtrack level
  //then call
  //sat_solver.cancelUntil(backtrack_level);
}
