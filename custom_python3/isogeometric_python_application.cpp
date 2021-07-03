/*
LICENSE: see isogeometric_application/LICENSE.txt
*/

//
//   Project Name:        Kratos
//   Last modified by:    $Author: hbui $
//   Date:                $Date: 2013-08-18 $
//   Revision:            $Revision: 1.1 $
//
//

// System includes

#if defined(KRATOS_PYTHON)
// External includes

#include "custom_utilities/patch_interface.h"
#include "custom_utilities/multipatch.h"
#include "custom_utilities/patch.h"
#include "custom_python3/add_utilities_to_python.h"
#include "custom_python3/add_control_point_to_python.h"
#include "custom_python3/add_control_grids_to_python.h"
#include "custom_python3/add_nurbs_to_python.h"
#include "custom_python3/add_pbbsplines_to_python.h"
#include "custom_python3/add_hbsplines_to_python.h"
#include "custom_python3/add_tsplines_to_python.h"
#include "custom_python3/add_fespace_to_python.hpp"
#include "custom_python3/add_grid_functions_to_python.hpp"
#include "custom_python3/add_patches_to_python.h"
#include "custom_python3/add_mesh_and_model_part_to_python.h"
#include "custom_python3/add_processes_to_python.h"
#include "custom_python3/add_io_to_python.h"
// #include "custom_python3/add_strategies_to_python.h"
// #ifdef ISOGEOMETRIC_USE_BREP
// #include "custom_python3/add_brep_and_level_set_to_python.h"
// #endif

////utilities

// Project includes
#include "includes/define_python.h"
#include "isogeometric_application.h"
#include "isogeometric_application_variables.h"


// Important: we don't want pybind to try to deference a `std::unique_ptr<Patch>`:
PYBIND11_MAKE_OPAQUE(typename Kratos::Patch<1>::UniquePointer);
PYBIND11_MAKE_OPAQUE(typename Kratos::Patch<2>::UniquePointer);
PYBIND11_MAKE_OPAQUE(typename Kratos::Patch<3>::UniquePointer);


namespace Kratos
{

namespace Python
{

PYBIND11_MODULE(KratosIsogeometricApplication, m)
{

    namespace py = pybind11;

    py::class_<KratosIsogeometricApplication,
           KratosIsogeometricApplication::Pointer,
           KratosApplication >(m, "KratosIsogeometricApplication")
           .def(py::init<>())
           ;

    IsogeometricApplication_AddBackendUtilitiesToPython(m);
    IsogeometricApplication_AddFrontendUtilitiesToPython(m);
    IsogeometricApplication_AddControlPointToPython(m);
    IsogeometricApplication_AddControlGridsToPython(m);
    IsogeometricApplication_AddFESpacesToPython<1>(m);
    IsogeometricApplication_AddFESpacesToPython<2>(m);
    IsogeometricApplication_AddFESpacesToPython<3>(m);
    IsogeometricApplication_AddGridFunctionsToPython<1>(m);
    IsogeometricApplication_AddGridFunctionsToPython<2>(m);
    IsogeometricApplication_AddGridFunctionsToPython<3>(m);
    IsogeometricApplication_AddPatchesToPython(m);
    IsogeometricApplication_AddNURBSToPython(m);
    IsogeometricApplication_AddPBBSplinesToPython(m);
    IsogeometricApplication_AddHBSplinesToPython(m);
    IsogeometricApplication_AddTSplinesToPython(m);
    IsogeometricApplication_AddMeshAndModelPartToPython(m);
    IsogeometricApplication_AddProcessesToPython(m);
    IsogeometricApplication_AddIOToPython(m);
    // IsogeometricApplication_AddStrategiesToPython(m);

    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, NURBS_WEIGHT )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, NURBS_WEIGHTS )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, NUM_DIVISION_1 )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, NUM_DIVISION_2 )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, NUM_DIVISION_3 )
    KRATOS_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS( m, LOCAL_COORDINATES )
    KRATOS_REGISTER_IN_PYTHON_3D_VARIABLE_WITH_COMPONENTS( m, CONTROL_POINT_COORDINATES )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, NUM_IGA_INTEGRATION_METHOD )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, CONTROL_POINT )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, KNOT_LEFT )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, KNOT_RIGHT )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, KNOT_TOP )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, KNOT_BOTTOM )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, KNOT_FRONT )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, KNOT_BACK )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, PATCH_INDEX )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, HIERARCHICAL_LEVEL )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, BASIS_FUNCTION_INDEX )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, EQUATION_INDEX )
    KRATOS_REGISTER_IN_PYTHON_VARIABLE( m, CELL_INDEX )

}


} // namespace Python.

} // namespace Kratos.

#endif // KRATOS_PYTHON defined
