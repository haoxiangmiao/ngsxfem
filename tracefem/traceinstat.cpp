/*
  
Solver for a parabolic pde

Solves

M du/dt  +  A u = f

by an implicite Euler method

*/



#include <solve.hpp>

using namespace ngsolve;
//using namespace ngfem;

//template<int D>
/*
  Every solver is a class derived from the class NumProc.
  It collects objects (such as bilinear-forms, gridfunctions) and parameters.
*/

class NumProcParabolic : public NumProc
{
protected:
  // bilinear-form for the stiffness matrix
  shared_ptr<BilinearForm> bfa;
  // bilinear-form for the mass-matrix
  shared_ptr<BilinearForm> bfm;
  // linear-form providing the right hand side
  shared_ptr<LinearForm> lff;
  // initial value
  //shared_ptr<LinearForm> lfu0;
  // solution vector
  shared_ptr<GridFunction> gfu;

  // time step
  double dt;
  // total time
  double tend;

  double periodicrhs;

public:
    
  /*
    In the constructor, the solver class gets the flags from the pde - input file.
    the PDE class apde constains all bilinear-forms, etc...
  */
  NumProcParabolic (shared_ptr<PDE> apde, const Flags & flags)
    : NumProc (apde)
  {
    // in the input-file, you specify the bilinear-forms for the stiffness and for the mass-term
    // like  "-bilinearforma=k". Default arguments are 'a' and 'm'

    bfa = apde->GetBilinearForm (flags.GetStringFlag ("bilinearforma", "a"));
    bfm = apde->GetBilinearForm (flags.GetStringFlag ("bilinearformm", "m"));
    lff = apde->GetLinearForm (flags.GetStringFlag ("linearform", "f"));
    //lfu0 = apde->GetLinearForm (flags.GetStringFlag ("linearformu0", "u0"));
    gfu = apde->GetGridFunction (flags.GetStringFlag ("gridfunction", "u"));
		
    dt = flags.GetNumFlag ("dt", 0.001);
    tend = flags.GetNumFlag ("tend", 1);

		periodicrhs = flags.GetNumFlag("periodic_rhs",0);
  }


  // solve at one level
  virtual void Do(LocalHeap & lh)
  {
    cout << "solve parabolic pde" << endl;
      
    // reference to the matrices provided by the bi-forms.
    // will be of type SparseSymmetricMatrix<double> for scalar problems

    const BaseMatrix & mata = bfa->GetMatrix();
    const BaseMatrix & matm = bfm->GetMatrix();
    const BaseVector & vecf = lff->GetVector();
    BaseVector & vecu = gfu->GetVector();

    // creates a matrix of same type:
    auto summat = matm.CreateMatrix();

    // create additional vectors:
    auto d = vecu.CreateVector();
    auto w = vecu.CreateVector();

		double per=1;

    // matrices matm and mata have the same memory layout. The arrays of values 
    // can be accessed and manipulated as vectors:

    summat->AsVector() = (1.0/dt) * matm.AsVector() + mata.AsVector();

    // A sparse matrix can compute a sparse factorization. 
    auto invmat = summat->InverseMatrix();
    // get initial value as grid vector
    //const BaseVector & vecu0 = lfu0->GetVector();
    //auto invmatm = matm.CreateMatrix();
    //invmatm->AsVector()=matm.AsVector();
    //auto invmatm2=invmatm->InverseMatrix();
    //vecu = *invmatm2*vecu0;
    // implicite Euler method
    for (double t = 0; t <= tend; t += dt)
      {
				if (periodicrhs!=0)
					per=cos(periodicrhs*t);
		
				cout << "t = " << t << endl;
				d = per * vecf - mata * vecu;
				w = *invmat * d;
				vecu += w;
				// update visualization
				Ng_Redraw ();	  
      }
  }


  virtual string GetClassName () const
  {
    return "Parabolic Solver (Demo)";
  }

  virtual void PrintReport (ostream & ost)
  {
    ost << GetClassName() << endl
	<< "Bilinear-form A = " << bfa->GetName() << endl
	<< "Bilinear-form M = " << bfm->GetName() << endl
	<< "Linear-form     = " << lff->GetName() << endl
	<< "Gridfunction    = " << gfu->GetName() << endl
	<< "dt              = " << dt << endl
	<< "tend            = " << tend << endl;
  }

  ///
  static void PrintDoc (ostream & ost)
  {
    ost << 
      "\n\nNumproc Parabolic:\n" \
      "------------------\n" \
      "Solves a parabolic partial differential equation by an implicite Euler method\n\n" \
      "Required flags:\n" 
      "-bilinearforma=<bfname>\n" 
      "    bilinear-form providing the stiffness matrix\n" \
      "-bilinearformm=<bfname>\n" 
      "    bilinear-form providing the mass matrix\n" \
      "-linearform=<lfname>\n" \
      "    linear-form providing the right hand side\n" \
      "-gridfunction=<gfname>\n" \
      "    grid-function to store the solution vector\n" 
      "-dt=<value>\n"
      "    time step\n"
      "-tend=<value>\n"
      "    total time\n"
	<< endl;
  }
};



// register the numproc 'parabolic' 
static RegisterNumProc<NumProcParabolic> npinit1("parabolic");



//////////////////////////parabolic3d

class NumProcParabolic3d : public NumProc
{
protected:
  // bilinear-form for the stiffness matrix
  shared_ptr<BilinearForm> bfa;
  // bilinear-form for the mass-matrix
  shared_ptr<BilinearForm> bfm;
  // linear-form providing the right hand side
  shared_ptr<LinearForm> lff;
  // initial value
  //shared_ptr<LinearForm> lfu0;
  // solution vector
  shared_ptr<GridFunction> gfu;

	//vtk output numproc
	shared_ptr<NumProc> npto;
  // time step
  double dt;
  // total time
  double tend;

  double periodicrhs;

public:
    
  /*
    In the constructor, the solver class gets the flags from the pde - input file.
    the PDE class apde constains all bilinear-forms, etc...
  */
  NumProcParabolic3d (shared_ptr<PDE> apde, const Flags & flags)
    : NumProc (apde)
  {
    // in the input-file, you specify the bilinear-forms for the stiffness and for the mass-term
    // like  "-bilinearforma=k". Default arguments are 'a' and 'm'

    bfa = apde->GetBilinearForm (flags.GetStringFlag ("bilinearforma", "a"));
    bfm = apde->GetBilinearForm (flags.GetStringFlag ("bilinearformm", "m"));
    lff = apde->GetLinearForm (flags.GetStringFlag ("linearform", "f"));
    //lfu0 = apde->GetLinearForm (flags.GetStringFlag ("linearformu0", "u0"));
    gfu = apde->GetGridFunction (flags.GetStringFlag ("gridfunction", "u"));
		npto = apde->GetNumProc (flags.GetStringFlag ("visnumproc"));
		
    dt = flags.GetNumFlag ("dt", 0.001);
    tend = flags.GetNumFlag ("tend", 1);
		periodicrhs = flags.GetNumFlag("periodic_rhs",0);
  }


  // solve at one level
  virtual void Do(LocalHeap & lh)
  {
    cout << "solve parabolic pde" << endl;
      
    // reference to the matrices provided by the bi-forms.
    // will be of type SparseSymmetricMatrix<double> for scalar problems

    const BaseMatrix & mata = bfa->GetMatrix();
    const BaseMatrix & matm = bfm->GetMatrix();
    const BaseVector & vecf = lff->GetVector();
    BaseVector & vecu = gfu->GetVector();

    // creates a matrix of same type:
    auto summat = matm.CreateMatrix();

    // create additional vectors:
    auto d = vecu.CreateVector();
    auto w = vecu.CreateVector();
		
		double per=1;
    // matrices matm and mata have the same memory layout. The arrays of values 
    // can be accessed and manipulated as vectors:

    summat->AsVector() = (1.0/dt) * matm.AsVector() + mata.AsVector();

    // A sparse matrix can compute a sparse factorization. 
    auto invmat = summat->InverseMatrix();
    // get initial value as grid vector
    //const BaseVector & vecu0 = lfu0->GetVector();
    //auto invmatm = matm.CreateMatrix();
    //invmatm->AsVector()=matm.AsVector();
    //auto invmatm2=invmatm->InverseMatrix();
    //vecu = *invmatm2*vecu0;
    // implicite Euler method
    for (double t = 0; t <= tend; t += dt)
      {
				if (periodicrhs!=0)
					per=(cos(periodicrhs*t)+1)/2;

				cout << "t = " << t << endl;
				d = per * vecf - mata * vecu;
				w = *invmat * d;
				vecu += w;
				npto->Do(lh);
				// update visualization
				Ng_Redraw ();	  
      }
		//npto->resettime();
  }


  virtual string GetClassName () const
  {
    return "Parabolic Solver (Demo)";
  }

  virtual void PrintReport (ostream & ost)
  {
    ost << GetClassName() << endl
	<< "Bilinear-form A = " << bfa->GetName() << endl
	<< "Bilinear-form M = " << bfm->GetName() << endl
	<< "Linear-form     = " << lff->GetName() << endl
	<< "Gridfunction    = " << gfu->GetName() << endl
	<< "dt              = " << dt << endl
	<< "tend            = " << tend << endl;
  }

  ///
  static void PrintDoc (ostream & ost)
  {
    ost << 
      "\n\nNumproc Parabolic:\n" \
      "------------------\n" \
      "Solves a parabolic partial differential equation by an implicite Euler method\n\n" \
      "Required flags:\n" 
      "-bilinearforma=<bfname>\n" 
      "    bilinear-form providing the stiffness matrix\n" \
      "-bilinearformm=<bfname>\n" 
      "    bilinear-form providing the mass matrix\n" \
      "-linearform=<lfname>\n" \
      "    linear-form providing the right hand side\n" \
      "-gridfunction=<gfname>\n" \
      "    grid-function to store the solution vector\n" 
      "-dt=<value>\n"
      "    time step\n"
      "-tend=<value>\n"
      "    total time\n"
	<< endl;
  }
};



// register the numproc 'parabolic' 
static RegisterNumProc<NumProcParabolic3d> npinit2("parabolic3d");

