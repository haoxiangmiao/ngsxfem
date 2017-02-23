from ngsolve import *
from xfem import *
from netgen.geom2d import unit_square
from netgen.csg import unit_cube

import time                                                

ngsglobals.msg_level = 1

def test_fes_timing_2D(quad=False):
    mesh = Mesh (unit_square.GenerateMesh(maxh=0.2,quad_dominated=quad))
    Vh = H1(mesh, order=1, dirichlet=[1,2,3,4])
    container = Vh.__timing__()
    ts = time.time()
    steps = 10
    for i in range(steps):
        Vh.Update()
    te = time.time()
    container.append(("Update",1e9*(te-ts)/steps))
    return container

def test_xfes_timing_2D(quad=False):
    mesh = Mesh (unit_square.GenerateMesh(maxh=0.2,quad_dominated=quad))
    lsetp1 = GridFunction(H1(mesh,order=1))
    InterpolateToP1((sqrt(sqrt(x*x+y*y)) - 1.0),lsetp1)
    Vh = H1(mesh, order=1, dirichlet=[1,2,3,4])
    Vhx = XFESpace(Vh, lsetp1)
    container = Vhx.__timing__()
    ts = time.time()
    steps = 10
    for i in range(steps):
        Vhx.Update()
    te = time.time()
    container.append(("Update",1e9*(te-ts)/steps))
    return container

def test_fes_timing_3D(quad=False):
    mesh = Mesh (unit_cube.GenerateMesh(maxh=0.2,quad_dominated=quad))
    Vh = H1(mesh, order=1, dirichlet=[1,2,3,4])
    container = Vh.__timing__()
    ts = time.time()
    steps = 10
    for i in range(steps):
        Vh.Update()
    te = time.time()
    container.append(("Update",1e9*(te-ts)/steps))
    return container

def test_xfes_timing_3D(quad=False):
    mesh = Mesh (unit_cube.GenerateMesh(maxh=0.2,quad_dominated=quad))
    lsetp1 = GridFunction(H1(mesh,order=1))
    InterpolateToP1((sqrt(sqrt(x*x+y*y)) - 1.0),lsetp1)
    Vh = H1(mesh, order=1, dirichlet=[1,2,3,4])
    Vhx = XFESpace(Vh, lsetp1)
    container = Vhx.__timing__()
    ts = time.time()
    steps = 10
    for i in range(steps):
        Vhx.Update()
    te = time.time()
    container.append(("Update",1e9*(te-ts)/steps))
    return container

print("\nCalculations: \n\n")
stiming2d = test_fes_timing_2D()
xtiming2d = test_xfes_timing_2D()
stiming3d = test_fes_timing_3D()
xtiming3d = test_xfes_timing_3D()

print("\n\nReport:")
print(" FES Timings 2D:\n",stiming2d)
print("XFES Timings 2D:\n",xtiming2d)
print(" FES Timings 3D:\n",stiming3d)
print("XFES Timings 3D:\n",xtiming3d)
