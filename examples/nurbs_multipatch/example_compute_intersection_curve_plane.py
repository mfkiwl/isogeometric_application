##################################################################
##################################################################
import sys
import os
##################################################################
##################################################################
#importing Kratos modules
from KratosMultiphysics import *
from KratosMultiphysics.BRepApplication import *
from KratosMultiphysics.IsogeometricApplication import *
kernel = Kernel()   #defining kernel

import geometry_factory

mpatch_export3 = MultiNURBSPatchMatlabExporter()

def main():
    fname = os.path.basename(__file__)
    name = os.path.splitext(fname)[0]

    points = [[0.0, 3.0, 0.0], [0.5, 1.0, 0.0], [1.0, 0.1, 0.0], [2.0, -1.0, 0.0], [3.0, -3.0, 0.0]]
    curve_ptr = geometry_factory.CreateCurve(points, 2)
    curve = curve_ptr.GetReference()
    curve.Id = 1
    mpatch_export3.Export(curve_ptr, name + "_curve.m")

    intersect_util = IsogeometricIntersectionUtility()
    A = 0.0
    B = 1.0
    C = 0.0
    D = 3.0 # -1.0
    [stat, point] = intersect_util.ComputeIntersection(0.5, curve_ptr, A, B, C, D, 30, 1.0e-6)
    print("stat:", stat) # 0 is OK
    print("point:", point)

    gf = curve.GridFunction(CONTROL_POINT)
    P = gf.GetValue(point)
    print("P:", str(P))

if __name__ == "__main__":
    main()

