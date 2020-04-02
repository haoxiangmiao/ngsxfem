"""
	Description:
	------------
	Sharp cube defined by six level sets, one for each face.
"""

# Import libraries
from netgen.csg import *
from ngsolve import *
from xfem import *
from xfem.mlset import *

# Mesh
geo = CSGeometry()
geo.Add(OrthoBrick(Pnt(-0.8,-0.8,-0.8), Pnt(0.8,0.8,0.8)))
mesh = Mesh(geo.GenerateMesh(maxh=0.3))

# Level sets
level_sets = (x - 0.5, x + 0.5, x - y, z - 0)
nr_ls = len(level_sets)
level_sets_p1 = tuple(GridFunction(H1(mesh,order=1)) for i in range(nr_ls))

for i, lset_p1 in enumerate(level_sets_p1):
    InterpolateToP1(level_sets[i], lset_p1)

# Integrate
line = DomainTypeArray(dtlist=[(NEG, POS, IF, IF)])

# Compute length
length = Integrate(levelset_domain={"levelset": level_sets_p1,
                                    "domain_type": line},
                   mesh=mesh, cf=1, order=0)

print("length = {:10.8f}".format(length))
print("length error = {:4.3e}".format(abs(length - sqrt(2))))
assert abs(length - sqrt(2)) < 1e-12
