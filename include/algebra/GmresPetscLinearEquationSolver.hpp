/*=========================================================================

  Program: FEMUS
  Module: PetscLinearEquationSolver
  Authors: Eugenio Aulisa, Simone Bnà
 
  Copyright (c) FEMTTU
  All rights reserved. 

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/

#ifndef __GmresPetscLinearEquationSolver_hpp__
#define __GmresPetscLinearEquationSolver_hpp__

#include "FEMTTUConfig.h"

#ifdef HAVE_PETSC

#ifdef HAVE_MPI
#include <mpi.h> 
#endif

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "LinearEquationSolver.hpp"
#include "PetscVector.hpp"
#include "PetscMatrix.hpp"
#include "PetscMacro.hpp"


namespace femus {

  /**
   * This class inherits the abstract class LinearEquationSolver. In this class the solver is implemented using the PETSc package
   */

  class GmresPetscLinearEquationSolver : public LinearEquationSolver {

  public:
  
    // Constructor --------------------------------------
    /**  Constructor. Initializes Petsc data structures */
    GmresPetscLinearEquationSolver (const unsigned &igrid, mesh *other_mesh);
  
    /// Destructor.
    ~GmresPetscLinearEquationSolver ();

    /// Release all memory and clear data structures.
    void clear ();

    /// Initialize data structures if not done so already plus much more
    void init (Mat& Amat, Mat &Pmat);


    void set_tolerances(const double &rtol, const double &atol,
			const double &divtol, const unsigned &maxits,const unsigned &index);
 
    // Solvers ------------------------------------------------------
    // ========================================================
    /// Call the GMRES smoother-solver using the PetscLibrary.
    std::pair< int, double> solve(const vector <unsigned> &VankaIndex,
				  const short unsigned &NSchurVar,const bool &Schur,
				  const bool &ksp_clean);
 
  
  private:
    // data ---------------------------------
    PC _pc;      ///< Preconditioner context
    KSP _ksp;    ///< Krylov subspace context
    PetscReal _rtol;
    PetscReal _abstol;
    PetscReal _dtol;
    PetscInt  _maxits;
    
    vector< vector <PetscInt> > _indexai;
    bool _indexai_init;
  
    vector <IS> _isA;
    vector <IS> _isB;
    
    Mat _Pmat;
    bool _Pmat_is_initialized;  

    // Setting --------------------------------------------
    ///  Set the user-specified solver stored in \p _solver_type
    void set_petsc_solver_type ();

    clock_t BuildIndex();

  
  };

  inline GmresPetscLinearEquationSolver::GmresPetscLinearEquationSolver (const unsigned &igrid, mesh* other_msh)
    : LinearEquationSolver(igrid, other_msh) {
        
    if(igrid==0){
      this->_preconditioner_type = MLU_PRECOND;
      this->_solver_type         = PREONLY;
    }
    else{
      if(_msh->_nprocs==1) {  
	this->_preconditioner_type = ILU_PRECOND;
      } 
      else {
	this->_preconditioner_type = BLOCK_JACOBI_PRECOND;
      }
      unsigned dim = _msh->GetDimension();
      unsigned base = pow(2,dim);
      unsigned exponent = 5 - dim;
    }

    _rtol   = 1.e-8;
    _abstol = 1.e-40;
    _dtol   = 1.e+50;
    _maxits = 4;
  
    _indexai_init=0;
  
    _Pmat_is_initialized = false;

  }

  // =============================================
  inline GmresPetscLinearEquationSolver::~GmresPetscLinearEquationSolver () {
    this->clear ();
  
    for(unsigned i=0;i<_isA.size();i++){
      ISDestroy(&_isA[i]); 	
    }
  
    for(unsigned i=0;i<_isB.size();i++){
      ISDestroy(&_isB[i]); 	
    }
   
  }



} //end namespace femus


#endif 
#endif 