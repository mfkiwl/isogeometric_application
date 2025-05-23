/*
see isogeometric_application/LICENSE.txt
*/

//
//   Project Name:        Kratos
//   Last Modified by:    $Author: hbui $
//   Date:                $Date: 28 Oct 2013 $
//   Revision:            $Revision: 1.0 $
//
//

#if !defined(KRATOS_ISOGEOMETRIC_MODEL_PART_IO_H_INCLUDED )
#define  KRATOS_ISOGEOMETRIC_MODEL_PART_IO_H_INCLUDED

// System includes
#include <string>
#include <fstream>
#include <set>

// External includes

// Project includes
#include "includes/define.h"
#include "includes/io.h"
#include "utilities/timer.h"
#include "custom_utilities/iga_define.h"

namespace Kratos
{

///@name Kratos Globals
///@{

///@}
///@name Type Definitions
///@{

///@}
///@name  Enum's
///@{

///@}
///@name  Functions
///@{

///@}
///@name Kratos Classes
///@{

/// An IO class for reading and writing a modelpart
/** This class writes all modelpart data including the meshes.
*/
class IsogeometricModelPartIO : public IO<>
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of IsogeometricModelPartIO
    KRATOS_CLASS_POINTER_DEFINITION(IsogeometricModelPartIO);

    typedef IO<> BaseType;

    typedef typename BaseType::NodeType NodeType;

    typedef typename BaseType::MeshType MeshType;

    typedef typename BaseType::NodesContainerType NodesContainerType;

    typedef typename BaseType::PropertiesContainerType PropertiesContainerType;

    typedef typename BaseType::ElementsContainerType ElementsContainerType;

    typedef typename BaseType::ConditionsContainerType ConditionsContainerType;

    typedef typename BaseType::ConnectivitiesContainerType ConnectivitiesContainerType;

    typedef typename std::vector<std::ofstream*> OutputFilesContainerType;

    typedef typename BaseType::SizeType SizeType;

    typedef typename ModelPart::CommunicatorType CommunicatorType;

#ifndef SD_APP_FORWARD_COMPATIBILITY
    typedef VariableComponent<VectorComponentAdaptor<array_1d<double, 3> > > array_1d_component_type;
#endif

    ///@}
    ///@name Life Cycle
    ///@{

    /// Constructor with  filenames.
    IsogeometricModelPartIO(std::string const& Filename)
        : mNumberOfLines(1)
        , mInputBaseName(Filename), mOutputBaseName(Filename)
        , mInputFilename(Filename + ".mdpa"), mOutputFilename(Filename + "_out.mdpa")
        , mInput(mInputFilename.c_str()),  mOutput(mOutputFilename.c_str())
    {
        if (!mInput)
        {
            KRATOS_ERROR << "Error opening input file " << mInputFilename;
        }
        if (!mOutput)
        {
            KRATOS_ERROR << "Error opening output file " << mOutputFilename;
        }

        Timer::SetOutputFile(Filename + ".time");

    }

    /// Constructor with input and output filenames.
    IsogeometricModelPartIO(std::string const& InputFilename, std::string const& OutputFilename)
        : mNumberOfLines(1)
        , mInputBaseName(InputFilename), mOutputBaseName(OutputFilename)
        , mInputFilename(InputFilename + ".mdpa"), mOutputFilename(OutputFilename + ".mdpa")
        , mInput(mInputFilename.c_str()), mOutput(mOutputFilename.c_str())
    {
        if (!mInput)
        {
            KRATOS_ERROR << "Error opening input file " << mInputFilename;
        }
        if (!mOutput)
        {
            KRATOS_ERROR << "Error opening output file " << mOutputFilename;
        }

        Timer::SetOutputFile(InputFilename + ".time");
    }

    /// Constructor with filenames.
//       IsogeometricModelPartIO(std::string const& InputFilename, std::string const& OutputFilename)
// 	: mNumberOfLines(0), mInput(std::ifstream(InputFilename.c_str())), mOutput(std::ofstream(OutputFilename.c_str()))
//       {
//       }

    /// Destructor.
    ~IsogeometricModelPartIO() override {}

    ///@}
    ///@name Operators
    ///@{

    ///@}
    ///@name Operations
    ///@{

    bool ReadNode(NodeType& rThisNode) override
    {
        KRATOS_ERROR << "Calling base class member. Please check the definition of derived class.";
    }

    bool ReadNodes(NodesContainerType& rThisNodes) override
    {
        KRATOS_TRY
        ResetInput();
        std::string word;
        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            ReadBlockName(word);
            if (word == "Nodes")
            {
                ReadNodesBlock(rThisNodes);
            }
            else
            {
                SkipBlock(word);
            }
        }

        return true;
        KRATOS_CATCH("")
    }

    SizeType ReadNodesNumber() override
    {
        KRATOS_TRY;
        ResetInput();
        std::string word;
        std::size_t num_nodes = 0;
        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            ReadBlockName(word);
            if (word == "Nodes")
            {
                num_nodes += CountNodesInBlock();
            }
            else
            {
                SkipBlock(word);
            }
        }

        return num_nodes;
        KRATOS_CATCH("");
    }

    void WriteNodes(NodesContainerType const& rThisNodes) override
    {
        mOutput << "Begin Nodes" << std::endl;
        for (NodesContainerType::const_iterator i_node = rThisNodes.begin() ; i_node != rThisNodes.end() ; i_node++)
        {
            mOutput << i_node->Id() << "\t" << i_node->X()  << "\t" << i_node->Y() << "\t" << i_node->Z() << std::endl;
        }
        mOutput << "End Nodes" << std::endl;
    }

    void ReadProperties(Properties& rThisProperties) override
    {
        KRATOS_ERROR << "Calling base class member. Please check the definition of derived class";
    }

    void ReadProperties(PropertiesContainerType& rThisProperties) override
    {
        KRATOS_TRY
        ResetInput();
        std::string word;
        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            ReadBlockName(word);
            if (word == "Properties")
            {
                ReadPropertiesBlock(rThisProperties);
            }
            else
            {
                SkipBlock(word);
            }
        }
        KRATOS_CATCH("")
    }

    virtual void WriteProperties(PropertiesContainerType& rThisProperties)
    {
        for (PropertiesContainerType::const_iterator i_properties = rThisProperties.begin() ; i_properties != rThisProperties.end() ; i_properties++)
        {
            mOutput << "Begin Properties " << i_properties->Id() << std::endl;
            i_properties->PrintData(mOutput);
            mOutput << std::endl;
            mOutput << "End Properties" << std::endl;
        }
    }

    void ReadElement(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, Element::Pointer& pThisElements) override
    {
        KRATOS_ERROR << "Calling base class member. Please check the definition of derived class";
    }

    void ReadElements(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ElementsContainerType& rThisElements) override
    {
        KRATOS_TRY
        ResetInput();
        std::string word;
        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            ReadBlockName(word);
            if (word == "Elements")
            {
                ReadElementsBlock(rThisNodes, rThisProperties, rThisElements);
            }
            else
            {
                SkipBlock(word);
            }
        }
        KRATOS_CATCH("")
    }

    SizeType ReadElementsConnectivities(ConnectivitiesContainerType& rElementsConnectivities) override
    {
        KRATOS_TRY

        SizeType number_of_elements = 0;
        ResetInput();
        std::string word;
        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            ReadBlockName(word);
            if (word == "Elements")
            {
                number_of_elements += ReadElementsConnectivitiesBlock(rElementsConnectivities);
            }
            else
            {
                SkipBlock(word);
            }
        }
        return number_of_elements;

        KRATOS_CATCH("")
    }

    void WriteElements(ElementsContainerType const& rThisElements) override
    {
        mOutput << "Begin Elements" << std::endl;
        mOutput << "End Elements" << std::endl;
    }

    void ReadConditions(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ConditionsContainerType& rThisConditions) override
    {
        KRATOS_TRY
        ResetInput();
        std::string word;
        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            ReadBlockName(word);
            if (word == "Conditions")
            {
                ReadConditionsBlock(rThisNodes, rThisProperties, rThisConditions);
            }
            else
            {
                SkipBlock(word);
            }
        }
        KRATOS_CATCH("")
    }

    SizeType ReadConditionsConnectivities(ConnectivitiesContainerType& rConditionsConnectivities) override
    {
        KRATOS_TRY

        SizeType number_of_elements = 0;
        ResetInput();
        std::string word;
        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            ReadBlockName(word);
            if (word == "Conditions")
            {
                number_of_elements += ReadConditionsConnectivitiesBlock(rConditionsConnectivities);
            }
            else
            {
                SkipBlock(word);
            }
        }
        return number_of_elements;

        KRATOS_CATCH("")
    }

    virtual void WriteConditions(ConditionsContainerType const& rThisConditions)
    {
        mOutput << "Begin Conditions" << std::endl;
        mOutput << "End Conditions" << std::endl;
    }

    void ReadInitialValues(NodesContainerType& rThisNodes, ElementsContainerType& rThisElements, ConditionsContainerType& rThisConditions) override
    {
        KRATOS_TRY
        ResetInput();
        std::string word;
        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            ReadBlockName(word);
            if (word == "NodalData")
            {
                ReadNodalDataBlock(rThisNodes);
            }
            else if (word == "ElementalData")
            {
                ReadElementalDataBlock(rThisElements);
            }
            else if (word == "ConditionalData")
            {
                ReadConditionalDataBlock(rThisConditions);
            }
            else
            {
                SkipBlock(word);
            }
        }
        KRATOS_CATCH("")
    }

//       void ReadGeometries(NodesContainerType& rThisNodes, GeometriesContainerType& rResults);

    void ReadMesh(MeshType & rThisMesh) override
    {
        KRATOS_ERROR << "IsogeometricModelPartIO does not implement this method.";
    }

    virtual void WriteMesh(MeshType & rThisMesh)
    {
        WriteProperties(rThisMesh.Properties());
        WriteNodes(rThisMesh.Nodes());
        WriteElements(rThisMesh.Elements());
        WriteConditions(rThisMesh.Conditions());
    }

    void ReadModelPart(ModelPart & rThisModelPart) override
    {
        KRATOS_TRY

        Timer::Start("Reading Input");

        ResetInput();
        std::string word;
        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            ReadBlockName(word);
            if (word == "ModelPartData")
            {
                ReadModelPartDataBlock(rThisModelPart);
            }
            else if (word == "Table")
            {
                ReadTableBlock(rThisModelPart.Tables());
            }
            else if (word == "Properties")
            {
                ReadPropertiesBlock(rThisModelPart.rProperties());
            }
            else if (word == "Nodes")
            {
                ReadNodesBlock(rThisModelPart);
            }
            else if (word == "Elements")
            {
                ReadElementsBlock(rThisModelPart);
            }
            else if (word == "Conditions")
            {
                ReadConditionsBlock(rThisModelPart);
            }
            else if (word == "NodalData")
            {
                ReadNodalDataBlock(rThisModelPart.Nodes());
            }
            else if (word == "ElementalData")
            {
                ReadElementalDataBlock(rThisModelPart.Elements());
            }
            else if (word == "ConditionalData")
            {
                ReadConditionalDataBlock(rThisModelPart.Conditions());
            }
            else if (word == "CommunicatorData")
            {
                ReadCommunicatorDataBlock(rThisModelPart.GetCommunicator(), rThisModelPart.Nodes());
                //Adding the elements and conditions to the communicator
                rThisModelPart.GetCommunicator().LocalMesh().Elements() = rThisModelPart.Elements();
                rThisModelPart.GetCommunicator().LocalMesh().Conditions() = rThisModelPart.Conditions();
            }
            else if (word == "Mesh")
            {
                ReadMeshBlock(rThisModelPart);
            }

        }
        std::cout << "lines read : " << mNumberOfLines;
        std::cout << std::endl;
        Timer::Stop("Reading Input");
        KRATOS_CATCH("")
    }

    virtual void WriteModelPart(ModelPart & rThisModelPart)
    {
        mOutput << "Begin ModelPartData" << std::endl;
        mOutput << "End ModelPartData" << std::endl;
        WriteMesh(rThisModelPart.GetMesh());
    }

    /// Read the input file and create the nodal connectivities graph, stored in CSR format.
    /**
     * This function produces input for Metis' nodal graph partitioning algorithms.
     * The nodal graph is stored as a (compressed) matrix where index (i,j) is non-zero if
     * there is an edge in the mesh joining nodes i and j (note that nodes are numbered from zero here,
     * to make integration with Metis simpler).
     * @param NodeIndices After call, will point to C array of size NumNodes+1 containing the
     * first CSR array: entries related to node k are stored between positions (*NodeIndices)[k]
     * and (*NodeIndices)[k+1] of *NodeConnectivities.
     * @param NodeConnectivities After call, will point to a C array of size (*NodeIndices)[NumNodes].
     * entries between (*NodeIndices)[k] and (*NodeIndices)[k+1] are a list of all nodes connected
     * to node k (counting from 0).
     * @return Number of nodes.
     */
    SizeType ReadNodalGraph(int **NodeIndices, int **NodeConnectivities) override
    {
        SizeType num_nodes = ReadNodesNumber();

        // 1. Allocate an auxiliary vector of vectors
        ConnectivitiesContainerType aux_connectivities(num_nodes);

        // 2. Fill the auxiliary vector by reading elemental and conditional connectivities
        ResetInput();
        std::string word;
        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            ReadBlockName(word);
            if (word == "Elements")
            {
                FillNodalConnectivitiesFromElementBlock(aux_connectivities);
            }
            else if (word == "Conditions")
            {
                FillNodalConnectivitiesFromConditionBlock(aux_connectivities);
            }
            else
            {
                SkipBlock(word);
            }
        }

        // 3. Sort each entry in the auxiliary connectivities vector, remove duplicates
        SizeType num_entries = 0;
        for (ConnectivitiesContainerType::iterator it = aux_connectivities.begin(); it != aux_connectivities.end(); ++it)
        {
            std::sort(it->begin(), it->end());
            std::vector<SizeType>::iterator unique_end = std::unique(it->begin(), it->end());
            it->resize(unique_end - it->begin());
            num_entries += it->size();
        }

        // 4. Write connectivity data in CSR format
        *NodeIndices = new int[num_nodes + 1];
        (*NodeIndices)[0] = 0;
        *NodeConnectivities = new int[num_entries];

        SizeType i = 0;
        SizeType aux_index = 0;

        for (ConnectivitiesContainerType::iterator it = aux_connectivities.begin(); it != aux_connectivities.end(); ++it)
        {
            for (std::vector<SizeType>::iterator entry_it = it->begin(); entry_it != it->end(); entry_it++)
            {
                (*NodeConnectivities)[aux_index++] = (*entry_it - 1);    // substract 1 to make Ids start from 0
            }
            (*NodeIndices)[++i] = aux_index;
        }

        return num_nodes;
    }

    void DivideInputToPartitions(SizeType NumberOfPartitions, GraphType const& DomainsColoredGraph,
                                 PartitionIndicesType const& NodesPartitions,
                                 PartitionIndicesType const& ElementsPartitions,
                                 PartitionIndicesType const& ConditionsPartitions,
                                 PartitionIndicesContainerType const& NodesAllPartitions,
                                 PartitionIndicesContainerType const& ElementsAllPartitions,
                                 PartitionIndicesContainerType const& ConditionsAllPartitions) override
    {
        KRATOS_TRY
        ResetInput();
        std::string word;
        OutputFilesContainerType output_files;

        for (SizeType i = 0 ; i < NumberOfPartitions ; i++)
        {
            std::stringstream buffer;
            buffer << mOutputBaseName << "_" << i << ".mdpa";
            std::ofstream* p_ofstream = new std::ofstream(buffer.str().c_str());
            if (!(*p_ofstream))
            {
                KRATOS_ERROR << "Error opening output file : " << buffer.str();
            }

            output_files.push_back(p_ofstream);
        }

        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            ReadBlockName(word);
            if (word == "ModelPartData")
            {
                DivideModelPartDataBlock(output_files);
            }
            else if (word == "Table")
            {
                DivideTableBlock(output_files);
            }
            else if (word == "Properties")
            {
                DividePropertiesBlock(output_files);
            }
            else if (word == "Nodes")
            {
                DivideNodesBlock(output_files, NodesAllPartitions);
            }
            else if (word == "Elements")
            {
                DivideElementsBlock(output_files, ElementsAllPartitions);
            }
            else if (word == "Conditions")
            {
                DivideConditionsBlock(output_files, ConditionsAllPartitions);
            }
            else if (word == "NodalData")
            {
                DivideNodalDataBlock(output_files, NodesAllPartitions);
            }
            else if (word == "ElementalData")
            {
                DivideElementalDataBlock(output_files, ElementsAllPartitions);
            }
            else if (word == "ConditionalData")
            {
                DivideConditionalDataBlock(output_files, ConditionsAllPartitions);
            }
            else if (word == "Mesh")
            {
                DivideMeshBlock(output_files, NodesAllPartitions, ElementsAllPartitions, ConditionsAllPartitions);
            }

        }

        WritePartitionIndices(output_files, NodesPartitions, NodesAllPartitions);

        WriteCommunicatorData(output_files, NumberOfPartitions, DomainsColoredGraph, NodesPartitions, ElementsPartitions, ConditionsPartitions, NodesAllPartitions, ElementsAllPartitions, ConditionsAllPartitions);
        std::cout << "lines read : " << mNumberOfLines;
        std::cout << std::endl;

        for (SizeType i = 0 ; i < NumberOfPartitions ; i++)
        {
            delete output_files[i];
        }
        KRATOS_CATCH("")
    }

    ///@}
    ///@name Access
    ///@{

    ///@}
    ///@name Inquiry
    ///@{

    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    std::string Info() const override
    {
    return "IsogeometricModelPartIO";
    }

    ///@}
    ///@name Friends
    ///@{

    ///@}

protected:
    ///@name Protected static Member Variables
    ///@{

    ///@}
    ///@name Protected member Variables
    ///@{

    ///@}
    ///@name Protected Operators
    ///@{

    ///@}
    ///@name Protected Operations
    ///@{

    ///@}
    ///@name Protected  Access
    ///@{

    ///@}
    ///@name Protected Inquiry
    ///@{

    ///@}
    ///@name Protected LifeCycle
    ///@{

    ///@}

private:
    ///@name Static Member Variables
    ///@{

    ///@}
    ///@name Member Variables
    ///@{

    SizeType mNumberOfLines;
    std::string mInputBaseName;
    std::string mOutputBaseName;
    std::string mInputFilename;
    std::string mOutputFilename;
    std::ifstream mInput;
    std::ofstream mOutput;

    ///@}
    ///@name Private Operators
    ///@{

    ///@}
    ///@name Private Operations
    ///@{

    std::string& ReadBlockName(std::string& rBlockName)
    {
        KRATOS_TRY

        CheckStatement("Begin", rBlockName);
        ReadWord(rBlockName);

        return rBlockName;

        KRATOS_CATCH("")
    }

    void SkipBlock(std::string const& BlockName)
    {
        KRATOS_TRY

        std::string word;

        while (!mInput.eof())
        {
            ReadWord(word);
            if (word == "End")
            {
                ReadWord(word);
                if (word == BlockName)
                {
                    break;
                }
            }

        }

        KRATOS_CATCH("")
    }

    bool CheckEndBlock(std::string const& BlockName, std::string& rWord)
    {
        if (rWord == "End")
        {
            ReadWord(rWord);
            CheckStatement(BlockName, rWord);
            return true;
        }

        return false;
    }

    void ReadModelPartDataBlock(ModelPart& rModelPart)
    {
        KRATOS_TRY

        std::string variable_name;

        while (!mInput.eof())
        {
            ReadWord(variable_name);
            if (CheckEndBlock("ModelPartData", variable_name))
            {
                break;
            }
            if (KratosComponents<Variable<double> >::Has(variable_name))
            {
                std::string value;
                double temp;

                ReadWord(value); // reading value
                ExtractValue(value, temp);
                rModelPart[KratosComponents<Variable<double> >::Get(variable_name)] = temp;
            }
            else if (KratosComponents<Variable<bool> >::Has(variable_name))
            {
                std::string value;
                bool temp;

                ReadWord(value); // reading value
                ExtractValue(value, temp);
                rModelPart[KratosComponents<Variable<bool> >::Get(variable_name)] = temp;
            }
            else if (KratosComponents<Variable<int> >::Has(variable_name))
            {
                std::string value;
                int temp;

                ReadWord(value); // reading value
                ExtractValue(value, temp);
                rModelPart[KratosComponents<Variable<int> >::Get(variable_name)] = temp;
            }
            else if (KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
            {
                Vector temp_vector; // defining a Vector because for array_1d the operator >> is not defined yet!
                ReadVectorialValue(temp_vector);
                rModelPart[KratosComponents<Variable<array_1d<double, 3> > >::Get(variable_name)] = temp_vector;
            }
            else if (KratosComponents<Variable<Matrix> >::Has(variable_name))
            {
                ReadVectorialValue(rModelPart[KratosComponents<Variable<Matrix> >::Get(variable_name)]);
            }
            else
            {
                KRATOS_ERROR << variable_name << " is not a valid variable!!!" << std::endl
                             << " [Line " << mNumberOfLines << " ]";
            }
        }

        KRATOS_CATCH("")
    }

    void ReadTableBlock(ModelPart::TablesContainerType& rTables)
    {
        KRATOS_TRY

        ModelPart::TableType temp_table;

        SizeType table_id;
        std::string word;

        ReadWord(word);
        ExtractValue(word, table_id);

        std::string variable_name;

        ReadWord(variable_name);

        if (!KratosComponents<Variable<double> >::Has(variable_name))
        {
            KRATOS_ERROR << variable_name << " is not a valid argument variable!!! Table only accepts double arguments." << std::endl
                         << " [Line " << mNumberOfLines << " ]";

        }

        ReadWord(variable_name);

        if (!KratosComponents<Variable<double> >::Has(variable_name))
        {
            KRATOS_ERROR << variable_name << " is not a valid value variable!!! Table only accepts double values." << std::endl
                         << " [Line " << mNumberOfLines << " ]";
        }

        while (!mInput.eof())
        {
            double x;
            double y;
            ReadWord(word);
            if (CheckEndBlock("Table", word))
            {
                break;
            }

            ExtractValue(word, x);
            ReadWord(word);
            ExtractValue(word, y);

            temp_table.PushBack(x, y);
        }

        rTables.insert(table_id, temp_table);

        KRATOS_CATCH("")
    }

    void ReadNodesBlock(NodesContainerType& rThisNodes)
    {
        KRATOS_TRY

#if defined(KRATOS_SD_REF_NUMBER_2)
        NodeType temp_node;
        SizeType temp_id;

        std::string word;

        SizeType number_of_nodes_read = 0;

        std::cout << "Reading Nodes : ";

        while (!mInput.eof())
        {
            ReadWord(word);
            if (CheckEndBlock("Nodes", word))
            {
                break;
            }

            ExtractValue(word, temp_id);
            temp_node.SetId(temp_id);
            ReadWord(word);
            ExtractValue(word, temp_node.X());
            ReadWord(word);
            ExtractValue(word, temp_node.Y());
            ReadWord(word);
            ExtractValue(word, temp_node.Z());

            temp_node.X0() = temp_node.X();
            temp_node.Y0() = temp_node.Y();
            temp_node.Z0() = temp_node.Z();

            rThisNodes.push_back(temp_node);
            number_of_nodes_read++;
        }
#elif defined(KRATOS_SD_REF_NUMBER_3)
        NodeType::Pointer temp_node = Kratos::make_intrusive< NodeType >( 0, 0.0, 0.0, 0.0);
        SizeType temp_id;

        std::string word;

        SizeType number_of_nodes_read = 0;

        std::cout << "Reading Nodes : ";

        while (!mInput.eof())
        {
            ReadWord(word);
            if (CheckEndBlock("Nodes", word))
            {
                break;
            }

            ExtractValue(word, temp_id);
            temp_node->SetId(temp_id);
            ReadWord(word);
            ExtractValue(word, temp_node->X());
            ReadWord(word);
            ExtractValue(word, temp_node->Y());
            ReadWord(word);
            ExtractValue(word, temp_node->Z());

            temp_node->X0() = temp_node->X();
            temp_node->Y0() = temp_node->Y();
            temp_node->Z0() = temp_node->Z();

            rThisNodes.push_back(temp_node);
            number_of_nodes_read++;
        }
#endif
        std::cout << number_of_nodes_read << " nodes read" << std::endl;

        unsigned int numer_of_nodes_read = rThisNodes.size();
        rThisNodes.Unique();
        if (rThisNodes.size() != numer_of_nodes_read)
        {
            std::cout << "attention! we read " << numer_of_nodes_read << " but there are only " << rThisNodes.size() << " non repeated nodes" << std::endl;
        }

        KRATOS_CATCH("")
    }

    void ReadNodesBlock(ModelPart& rModelPart)
    {
        KRATOS_TRY

#if defined(KRATOS_SD_REF_NUMBER_2)
        NodeType temp_node;
        SizeType temp_id;

        // Giving model part's variables list to the node
        temp_node.SetSolutionStepVariablesList(&rModelPart.GetNodalSolutionStepVariablesList());

        //set buffer size
        temp_node.SetBufferSize(rModelPart.GetBufferSize());

        std::string word;

        SizeType number_of_nodes_read = 0;

        std::cout << "Reading Nodes : ";

        while (!mInput.eof())
        {
            ReadWord(word);
            if (CheckEndBlock("Nodes", word))
            {
                break;
            }

            ExtractValue(word, temp_id);
            temp_node.SetId(temp_id);
            ReadWord(word);
            ExtractValue(word, temp_node.X());
            ReadWord(word);
            ExtractValue(word, temp_node.Y());
            ReadWord(word);
            ExtractValue(word, temp_node.Z());

            temp_node.X0() = temp_node.X();
            temp_node.Y0() = temp_node.Y();
            temp_node.Z0() = temp_node.Z();

            rModelPart.Nodes().push_back(temp_node);
            number_of_nodes_read++;
        }
#elif defined(KRATOS_SD_REF_NUMBER_3)
        NodeType::Pointer temp_node = Kratos::make_intrusive< NodeType >( 0, 0.0, 0.0, 0.0);
        SizeType temp_id;

        // Giving model part's variables list to the node
        temp_node->SetSolutionStepVariablesList(&rModelPart.GetNodalSolutionStepVariablesList());

        //set buffer size
        temp_node->SetBufferSize(rModelPart.GetBufferSize());

        std::string word;

        SizeType number_of_nodes_read = 0;

        std::cout << "Reading Nodes : ";

        while (!mInput.eof())
        {
            ReadWord(word);
            if (CheckEndBlock("Nodes", word))
            {
                break;
            }

            ExtractValue(word, temp_id);
            temp_node->SetId(temp_id);
            ReadWord(word);
            ExtractValue(word, temp_node->X());
            ReadWord(word);
            ExtractValue(word, temp_node->Y());
            ReadWord(word);
            ExtractValue(word, temp_node->Z());

            temp_node->X0() = temp_node->X();
            temp_node->Y0() = temp_node->Y();
            temp_node->Z0() = temp_node->Z();

            rModelPart.Nodes().push_back(temp_node);
            number_of_nodes_read++;
        }
#endif

        std::cout << number_of_nodes_read << " nodes read" << std::endl;

        unsigned int numer_of_nodes_read = rModelPart.Nodes().size();
        rModelPart.Nodes().Unique();
        if (rModelPart.Nodes().size() != numer_of_nodes_read)
        {
            std::cout << "attention! we read " << numer_of_nodes_read << " but there are only " << rModelPart.Nodes().size() << " non repeated nodes" << std::endl;
        }

        KRATOS_CATCH("")
    }

    std::size_t CountNodesInBlock()
    {
        KRATOS_TRY;

        std::vector<SizeType> found_ids;

        SizeType temp_id;

        std::string word;

        SizeType number_of_nodes_read = 0;

        //std::cout << "Reading Nodes : ";

        while (!mInput.eof())
        {
            ReadWord(word);
            if (CheckEndBlock("Nodes", word))
            {
                break;
            }

            ExtractValue(word, temp_id);
            found_ids.push_back(temp_id);

            ReadWord(word); // skip X coordinate
            ReadWord(word); // skip Y
            ReadWord(word); // skip Z

            number_of_nodes_read++;
        }
        //std::cout << number_of_nodes_read << " nodes read" << std::endl;

        // Error check: look for duplicate nodes
        std::sort(found_ids.begin(), found_ids.end());
        std::vector<std::size_t>::iterator unique_end = std::unique(found_ids.begin(), found_ids.end());
        std::size_t number_of_unique_nodes = std::distance(found_ids.begin(), unique_end);

        if (number_of_unique_nodes != number_of_nodes_read)
        {
            std::cout << "attention! we read " << number_of_nodes_read << " but there are only " << number_of_unique_nodes << " non repeated nodes" << std::endl;
        }

        return number_of_nodes_read;

        KRATOS_CATCH("");
    }

    void ReadPropertiesBlock(PropertiesContainerType& rThisProperties)
    {
        KRATOS_TRY

#if defined(KRATOS_SD_REF_NUMBER_2)
        Properties temp_properties;
#elif defined(KRATOS_SD_REF_NUMBER_3)
        Properties::Pointer props = iga::make_shared<Properties>();
        Properties& temp_properties = *props;
#endif

        std::string word;
        std::string variable_name;

        SizeType temp_properties_id;

        ReadWord(word);
        ExtractValue(word, temp_properties_id);
        temp_properties.SetId(temp_properties_id);

        while (!mInput.eof())
        {
            ReadWord(variable_name);
            if (CheckEndBlock("Properties", variable_name))
            {
                break;
            }

            if (KratosComponents<Variable<std::string> >::Has(variable_name))
            {
                std::string value;
                std::string  temp;

                ReadWord(value); // reading value
                ExtractValue(value, temp);
                temp_properties[KratosComponents<Variable<std::string> >::Get(variable_name)] = temp;
            }
            else if (KratosComponents<Variable<double> >::Has(variable_name))
            {
                std::string value;
                double temp;

                ReadWord(value); // reading value
                ExtractValue(value, temp);
                temp_properties[KratosComponents<Variable<double> >::Get(variable_name)] = temp;
            }
            else if (KratosComponents<Variable<int> >::Has(variable_name))
            {
                std::string value;
                int temp;

                ReadWord(value); // reading value
                ExtractValue(value, temp);
                temp_properties[KratosComponents<Variable<int> >::Get(variable_name)] = temp;
            }
            else if (KratosComponents<Variable<bool> >::Has(variable_name))
            {
                std::string value;
                bool temp;

                ReadWord(value); // reading value
                ExtractValue(value, temp);
                temp_properties[KratosComponents<Variable<bool> >::Get(variable_name)] = temp;
            }
            else if (KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
            {
                Vector temp_vector; // defining a Vector because for array_1d the operator >> is not defined yet!
                ReadVectorialValue(temp_vector);
                temp_properties[KratosComponents<Variable<array_1d<double, 3> > >::Get(variable_name)] = temp_vector;
            }
            else if (KratosComponents<Variable<Vector> >::Has(variable_name))
            {
                ReadVectorialValue(temp_properties[KratosComponents<Variable<Vector> >::Get(variable_name)]);
            }
            else if (KratosComponents<Variable<Matrix> >::Has(variable_name))
            {
                ReadVectorialValue(temp_properties[KratosComponents<Variable<Matrix> >::Get(variable_name)]);
            }
            else
            {
                KRATOS_ERROR << variable_name << " is not a valid variable!!!" << std::endl
                             << " [Line " << mNumberOfLines << " ]";
            }
        }

#if defined(KRATOS_SD_REF_NUMBER_2)
        rThisProperties.push_back(temp_properties);
#elif defined(KRATOS_SD_REF_NUMBER_3)
        rThisProperties.push_back(props);
#endif

        KRATOS_CATCH("")
    }

    void ReadElementsBlock(ModelPart& rModelPart)
    {
        ReadElementsBlock(rModelPart.Nodes(), rModelPart.rProperties(), rModelPart.Elements());
    }

//    void ReadElementsBlock(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ElementsContainerType& rThisElements)
//    {
//        KRATOS_TRY

//        SizeType id;
//        SizeType properties_id;
//        SizeType node_id;
//        SizeType number_of_read_elements = 0;

//        std::string word;
//        std::string element_name;

//        ReadWord(element_name);
//        std::cout << "Reading Elements : ";

//        if(!KratosComponents<Element>::Has(element_name))
//        {
//            KRATOS_ERROR << "Element " << element_name << " is not registered in Kratos."
//                         << " Please check the spelling of the element name and see if the application which containing it, is registered corectly."
//                         << " [Line " << mNumberOfLines << " ]";
//        }

//        Element const& r_clone_element = KratosComponents<Element>::Get(element_name);
//        SizeType number_of_nodes = r_clone_element.GetGeometry().size();
//        Element::NodesArrayType temp_element_nodes;

//        while(!mInput.eof())
//        {
//            ReadWord(word); // Reading the element id or End
//            if(CheckEndBlock("Elements", word))
//                break;

//            ExtractValue(word,id);
//            ReadWord(word); // Reading the properties id;
//            ExtractValue(word, properties_id);
//            Properties::Pointer p_temp_properties = *(FindKey(rThisProperties, properties_id, "Properties").base());
//            temp_element_nodes.clear();
//            for(SizeType i = 0 ; i < number_of_nodes ; i++)
//            {
//                ReadWord(word); // Reading the node id;
//                ExtractValue(word, node_id);
//                temp_element_nodes.push_back( *(FindKey(rThisNodes, node_id, "Node").base()));
//            }

//            rThisElements.push_back(r_clone_element.Create(id, temp_element_nodes, p_temp_properties));
//            number_of_read_elements++;

//        }
//        std::cout << number_of_read_elements << " " << element_name << " read" << std::endl;
//        rThisElements.Unique();

//        KRATOS_CATCH("")
//    }

    // hbui modify to read isogeometric element
    // Note that: this implementation cannot handle the comments properly
    void ReadElementsBlock(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ElementsContainerType& rThisElements)
    {
        KRATOS_TRY

        SizeType id;
        SizeType properties_id;
        SizeType node_id;
        SizeType number_of_read_elements = 0;

        std::string word;
        std::string element_name;

        ReadWord(element_name);
        std::cout << "Reading Elements : ";

        if (!KratosComponents<Element>::Has(element_name))
        {
            KRATOS_ERROR << "Element " << element_name << " is not registered in Kratos."
                         << " Please check the spelling of the element name and see if the application which containing it, is registered corectly."
                         << " [Line " << mNumberOfLines << " ]";
        }

        Element const& r_clone_element = KratosComponents<Element>::Get(element_name);

//        KRATOS_WATCH("clone element created")

        SizeType number_of_nodes;
        std::vector<int> temp_node_ids;
        Element::NodesArrayType temp_element_nodes;

        int CurrentNumberOfLine = mNumberOfLines;
        while (!mInput.eof())
        {
            ReadWord(word); // Reading the element id or End
            if (CheckEndBlock("Elements", word))
            {
                break;
            }

            ExtractValue(word, id);
            ReadWord(word); // Reading the properties id;
            ExtractValue(word, properties_id);
            Properties::Pointer p_temp_properties = *(FindKey(rThisProperties, properties_id, "Properties").base());

//            KRATOS_WATCH("temp properties created")

            temp_node_ids.clear();

            while (mNumberOfLines == CurrentNumberOfLine)
            {
                ReadWord2(word); // Reading the node id
                if (!word.empty())
                {
                    ExtractValue(word, node_id);
                    temp_node_ids.push_back(node_id);
                }
            }

            temp_element_nodes.clear();
            for (int i = 0; i < temp_node_ids.size(); i++)
            {
                temp_element_nodes.push_back( *(FindKey(rThisNodes, temp_node_ids[i], "Node").base()));
            }

            Element::Pointer p_temp_element = r_clone_element.Create(id, temp_element_nodes, p_temp_properties);

//            KRATOS_WATCH("element created")

            rThisElements.push_back(p_temp_element);
            number_of_read_elements++;

            CurrentNumberOfLine = mNumberOfLines;
        }

        std::cout << number_of_read_elements << " " << element_name << " read" << std::endl;

        rThisElements.Unique();

        KRATOS_CATCH("")
    }

    void ReadConditionsBlock(ModelPart& rModelPart)
    {
        ReadConditionsBlock(rModelPart.Nodes(), rModelPart.rProperties(), rModelPart.Conditions());
    }

//    void ReadConditionsBlock(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ConditionsContainerType& rThisConditions)
//    {
//        KRATOS_TRY

//        SizeType id;
//        SizeType properties_id;
//        SizeType node_id;

//        std::string word;
//        std::string condition_name;

//        ReadWord(condition_name);
//        if(!KratosComponents<Condition>::Has(condition_name))
//        {
//            KRATOS_ERROR << "Condition " << condition_name << " is not registered in Kratos."
//                         << " Please check the spelling of the condition name and see if the application containing it is registered corectly."
//                         << " [Line " << mNumberOfLines << " ]";
//        }

//        Condition const& r_clone_condition = KratosComponents<Condition>::Get(condition_name);
//        SizeType number_of_nodes = r_clone_condition.GetGeometry().size();
//        Condition::NodesArrayType temp_condition_nodes;

//        while(!mInput.eof())
//        {
//            ReadWord(word); // Reading the condition id or End
//            if(CheckEndBlock("Conditions", word))
//                break;

//            ExtractValue(word,id);
//            ReadWord(word); // Reading the properties id;
//            ExtractValue(word, properties_id);
//            Properties::Pointer p_temp_properties = *(FindKey(rThisProperties, properties_id, "Properties").base());
//            temp_condition_nodes.clear();
//            for(SizeType i = 0 ; i < number_of_nodes ; i++)
//            {
//                ReadWord(word); // Reading the node id;
//                ExtractValue(word, node_id);
//                temp_condition_nodes.push_back( *(FindKey(rThisNodes, node_id, "Node").base()));
//            }

//            rThisConditions.push_back(r_clone_condition.Create(id, temp_condition_nodes, p_temp_properties));
//        }
//        rThisConditions.Unique();

//        KRATOS_CATCH("")
//    }

    // hbui modify to read isogeometric condition
    // Note that: this implementation cannot handle the comments properly
    void ReadConditionsBlock(NodesContainerType& rThisNodes, PropertiesContainerType& rThisProperties, ConditionsContainerType& rThisConditions)
    {
        KRATOS_TRY

        SizeType id;
        SizeType properties_id;
        SizeType node_id;
        SizeType number_of_read_conditions = 0;

        std::string word;
        std::string condition_name;

        ReadWord(condition_name);
        std::cout << "Reading Conditions : ";

        if (!KratosComponents<Condition>::Has(condition_name))
        {
            KRATOS_ERROR << "Condition " << condition_name << " is not registered in Kratos."
                         << " Please check the spelling of the condition name and see if the application containing it is registered corectly."
                         << " [Line " << mNumberOfLines << " ]";
        }

        Condition const& r_clone_condition = KratosComponents<Condition>::Get(condition_name);
        SizeType number_of_nodes;
        std::vector<int> temp_node_ids;
        Condition::NodesArrayType temp_condition_nodes;

        int CurrentNumberOfLine = mNumberOfLines;
        while (!mInput.eof())
        {
            ReadWord(word); // Reading the condition id or End
            if (CheckEndBlock("Conditions", word))
            {
                break;
            }

            ExtractValue(word, id);
            ReadWord(word); // Reading the properties id;
            ExtractValue(word, properties_id);
            Properties::Pointer p_temp_properties = *(FindKey(rThisProperties, properties_id, "Properties").base());
            temp_node_ids.clear();

            while (mNumberOfLines == CurrentNumberOfLine)
            {
                ReadWord2(word); // Reading the node id
                if (!word.empty())
                {
                    ExtractValue(word, node_id);
                    temp_node_ids.push_back(node_id);
                }
            }

            temp_condition_nodes.clear();
            for (int i = 0; i < temp_node_ids.size(); i++)
            {
                temp_condition_nodes.push_back( *(FindKey(rThisNodes, temp_node_ids[i], "Node").base()));
            }

            Condition::Pointer p_temp_condition = r_clone_condition.Create(id, temp_condition_nodes, p_temp_properties);

            rThisConditions.push_back(p_temp_condition);
            number_of_read_conditions++;

            CurrentNumberOfLine = mNumberOfLines;
        }

        std::cout << number_of_read_conditions << " " << condition_name << " read" << std::endl;

        rThisConditions.Unique();

        KRATOS_CATCH("")
    }

    void ReadNodalDataBlock(NodesContainerType& rThisNodes)
    {
        KRATOS_TRY

        std::string variable_name;

        ReadWord(variable_name);

        if (KratosComponents<Flags >::Has(variable_name))
        {
            ReadNodalFlags(rThisNodes, static_cast<Flags const& >(KratosComponents<Flags >::Get(variable_name)));
        }
        else if (KratosComponents<Variable<int> >::Has(variable_name))
        {
            ReadNodalScalarVariableData(rThisNodes, static_cast<Variable<int> const& >(KratosComponents<Variable<int> >::Get(variable_name)));
        }
        else if (KratosComponents<Variable<double> >::Has(variable_name))
        {
            ReadNodalDofVariableData(rThisNodes, static_cast<Variable<double> const& >(KratosComponents<Variable<double> >::Get(variable_name)));
        }
#ifndef SD_APP_FORWARD_COMPATIBILITY
        else if (KratosComponents<array_1d_component_type>::Has(variable_name))
        {
            ReadNodalDofVariableData(rThisNodes, static_cast<array_1d_component_type const& >(KratosComponents<array_1d_component_type>::Get(variable_name)));
        }
#endif
        else if (KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
        {
            ReadNodalVectorialVariableData(rThisNodes, static_cast<Variable<array_1d<double, 3> > const& >(KratosComponents<Variable<array_1d<double, 3> > >::Get(variable_name)), Vector(3));
        }
        else if (KratosComponents<Variable<Vector> >::Has(variable_name))
        {
            ReadNodalVectorialVariableData(rThisNodes, static_cast<Variable<Vector > const& >(KratosComponents<Variable<Vector> >::Get(variable_name)), Vector());
        }
        else if (KratosComponents<Variable<Matrix> >::Has(variable_name))
        {
            ReadNodalVectorialVariableData(rThisNodes, static_cast<Variable<Matrix > const& >(KratosComponents<Variable<Matrix> >::Get(variable_name)), Matrix());
        }
        else if (KratosComponents<VariableData>::Has(variable_name))
        {
            KRATOS_ERROR << variable_name << " is not supported to be read by this IO or the type of variable is not registered correctly" << std::endl
                         << " [Line " << mNumberOfLines << " ]";
        }
        else
        {
            KRATOS_ERROR << variable_name << " is not a valid variable!!!" << std::endl
                         << " [Line " << mNumberOfLines << " ]";
        }

        KRATOS_CATCH("")
    }

    template<class TVariableType>
    void ReadNodalDofVariableData(NodesContainerType& rThisNodes, TVariableType& rVariable)
    {
        KRATOS_TRY

        SizeType id;
        bool is_fixed;
        double nodal_value;

        std::string value;

        while (!mInput.eof())
        {
            ReadWord(value); // reading id
            if (CheckEndBlock("NodalData", value))
            {
                break;
            }

            ExtractValue(value, id);
            typename NodesContainerType::iterator i_node = FindKey(rThisNodes, id, "Node");

            // readidng is_fixed
            ReadWord(value);
            ExtractValue(value, is_fixed);
            if (is_fixed)
            {
                i_node->Fix(rVariable);
            }

            // readidng nodal_value
            ReadWord(value);
            ExtractValue(value, nodal_value);

            i_node->GetSolutionStepValue(rVariable, 0) =  nodal_value;
        }

        KRATOS_CATCH("")
    }

    void ReadNodalFlags(NodesContainerType& rThisNodes, Flags const& rFlags)
    {

        KRATOS_TRY

        SizeType id;

        std::string value;

        while (!mInput.eof())
        {
            ReadWord(value); // reading id
            if (CheckEndBlock("NodalData", value))
            {
                break;
            }

            ExtractValue(value, id);

            FindKey(rThisNodes, id, "Node")->Set(rFlags);
        }

        KRATOS_CATCH("")
    }

    template<class TVariableType>
    void ReadNodalScalarVariableData(NodesContainerType& rThisNodes, TVariableType& rVariable)
    {
        KRATOS_TRY

        SizeType id;
        bool is_fixed;
        typename TVariableType::Type nodal_value;

        std::string value;

        while (!mInput.eof())
        {
            ReadWord(value); // reading id
            if (CheckEndBlock("NodalData", value))
            {
                break;
            }

            ExtractValue(value, id);

            // readidng is_fixed
            ReadWord(value);
            ExtractValue(value, is_fixed);
            if (is_fixed)
            {
                KRATOS_ERROR << "Only double variables or components can be fixed."
                             <<  " [Line " << mNumberOfLines << " ]";
            }

            // readidng nodal_value
            ReadWord(value);
            ExtractValue(value, nodal_value);

            FindKey(rThisNodes, id, "Node")->GetSolutionStepValue(rVariable, 0) =  nodal_value;
        }

        KRATOS_CATCH("")
    }

    template<class TVariableType, class TDataType>
    void ReadNodalVectorialVariableData(NodesContainerType& rThisNodes, TVariableType& rVariable, TDataType Dummy)
    {
        KRATOS_TRY

        SizeType id;
        bool is_fixed;
        TDataType nodal_value;

        std::string value;

        while (!mInput.eof())
        {
            ReadWord(value); // reading id
            if (CheckEndBlock("NodalData", value))
            {
                break;
            }

            ExtractValue(value, id);

            // readidng is_fixed
            ReadWord(value);
            ExtractValue(value, is_fixed);
            if (is_fixed)
            {
                KRATOS_ERROR << "Only double variables or components can be fixed."
                             <<  " [Line " << mNumberOfLines << " ]";
            }

            // readidng nodal_value
            ReadVectorialValue(nodal_value);

            FindKey(rThisNodes, id, "Node")->GetSolutionStepValue(rVariable, 0) =  nodal_value;
        }

        KRATOS_CATCH("")
    }

    void ReadElementalDataBlock(ElementsContainerType& rThisElements)
    {
        KRATOS_TRY

        std::string variable_name;

        ReadWord(variable_name);

        if (KratosComponents<Variable<bool> >::Has(variable_name))
        {
            ReadElementalScalarVariableData(rThisElements, static_cast<Variable<bool> const& >(KratosComponents<Variable<bool> >::Get(variable_name)));
        }
        else if (KratosComponents<Variable<int> >::Has(variable_name))
        {
            ReadElementalScalarVariableData(rThisElements, static_cast<Variable<int> const& >(KratosComponents<Variable<int> >::Get(variable_name)));
        }
        else if (KratosComponents<Variable<double> >::Has(variable_name))
        {
            ReadElementalScalarVariableData(rThisElements, static_cast<Variable<double> const& >(KratosComponents<Variable<double> >::Get(variable_name)));
        }
#ifndef SD_APP_FORWARD_COMPATIBILITY
        else if (KratosComponents<array_1d_component_type>::Has(variable_name))
        {
            ReadElementalScalarVariableData(rThisElements, static_cast<array_1d_component_type const& >(KratosComponents<array_1d_component_type>::Get(variable_name)));
        }
#endif
        else if (KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
        {
            ReadElementalVectorialVariableData(rThisElements, static_cast<Variable<array_1d<double, 3> > const& >(KratosComponents<Variable<array_1d<double, 3> > >::Get(variable_name)), Vector(3));
        }
        else if (KratosComponents<Variable<Vector> >::Has(variable_name))
        {
            ReadElementalVectorialVariableData(rThisElements, static_cast<Variable<Vector > const& >(KratosComponents<Variable<Vector> >::Get(variable_name)), Vector());
        }
        else if (KratosComponents<Variable<Matrix> >::Has(variable_name))
        {
            ReadElementalVectorialVariableData(rThisElements, static_cast<Variable<Matrix > const& >(KratosComponents<Variable<Matrix> >::Get(variable_name)), Matrix());
        }
        else
        {
            KRATOS_ERROR << variable_name << " is not a valid variable!!!" << std::endl
                         << " [Line " << mNumberOfLines << " ]";
        }

        KRATOS_CATCH("")
    }

    template<class TVariableType>
    void ReadElementalScalarVariableData(ElementsContainerType& rThisElements, TVariableType& rVariable)
    {
        KRATOS_TRY

        SizeType id;
        double elemental_value;

        std::string value;

        while (!mInput.eof())
        {
            ReadWord(value); // reading id
            if (CheckEndBlock("ElementalData", value))
            {
                break;
            }

            ExtractValue(value, id);

            // readidng nodal_value
            ReadWord(value);
            ExtractValue(value, elemental_value);

            ModelPart::ElementIterator i_result = rThisElements.find(id);
            if (i_result != rThisElements.end())
            {
                i_result->GetValue(rVariable) =  elemental_value;
            }
            //hbui: turn this off for not flush out the screen when running trilinos application
//            else
//                std::cout  << "WARNING! Assigning " << rVariable.Name() << " to not existing element #" << id << " [Line " << mNumberOfLines << " ]" << std::endl;
        }

        KRATOS_CATCH("")
    }

    template<class TVariableType, class TDataType>
    void ReadElementalVectorialVariableData(ElementsContainerType& rThisElements, TVariableType& rVariable, TDataType Dummy)
    {
        KRATOS_TRY

        SizeType id;
        TDataType elemental_value;

        std::string value;

        while (!mInput.eof())
        {
            ReadWord(value); // reading id
            if (CheckEndBlock("ElementalData", value))
            {
                break;
            }

            ExtractValue(value, id);

            // reading nodal_value
            ReadVectorialValue(elemental_value);
            ExtractValue(value, elemental_value); //?what's this

            ModelPart::ElementIterator i_result = rThisElements.find(id);
            if (i_result != rThisElements.end())
            {
                i_result->GetValue(rVariable) =  elemental_value;
            }
            //hbui: turn this off for not flush out the screen when running trilinos application
//            else
//                std::cout  << "WARNING! Assigning " << rVariable.Name() << " to not existing element #" << id << " [Line " << mNumberOfLines << " ]" << std::endl;
        }

        KRATOS_CATCH("")
    }
    void ReadConditionalDataBlock(ConditionsContainerType& rThisConditions)
    {
        KRATOS_TRY

        std::string variable_name;

        ReadWord(variable_name);

        if (KratosComponents<Variable<double> >::Has(variable_name))
        {
            ReadConditionalScalarVariableData(rThisConditions, static_cast<Variable<double> const& >(KratosComponents<Variable<double> >::Get(variable_name)));
        }
        else if (KratosComponents<Variable<bool> >::Has(variable_name))
        {
            ReadConditionalScalarVariableData(rThisConditions, static_cast<Variable<bool> const& >(KratosComponents<Variable<bool> >::Get(variable_name)));
        }
        else if (KratosComponents<Variable<int> >::Has(variable_name))
        {
            ReadConditionalScalarVariableData(rThisConditions, static_cast<Variable<int> const& >(KratosComponents<Variable<int> >::Get(variable_name)));
        }
#ifndef SD_APP_FORWARD_COMPATIBILITY
        else if (KratosComponents<array_1d_component_type>::Has(variable_name))
        {
            ReadConditionalScalarVariableData(rThisConditions, static_cast<array_1d_component_type const& >(KratosComponents<array_1d_component_type>::Get(variable_name)));
        }
#endif
        else if (KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
        {
            ReadConditionalVectorialVariableData(rThisConditions, static_cast<Variable<array_1d<double, 3> > const& >(KratosComponents<Variable<array_1d<double, 3> > >::Get(variable_name)), Vector(3));
        }
        else if (KratosComponents<Variable<Vector> >::Has(variable_name))
        {
            ReadConditionalVectorialVariableData(rThisConditions, static_cast<Variable<Vector > const& >(KratosComponents<Variable<Vector> >::Get(variable_name)), Vector());
        }
        else if (KratosComponents<Variable<Matrix> >::Has(variable_name))
        {
            ReadConditionalVectorialVariableData(rThisConditions, static_cast<Variable<Matrix > const& >(KratosComponents<Variable<Matrix> >::Get(variable_name)), Matrix());
        }
        else
        {
            KRATOS_ERROR << variable_name << " is not a valid variable!!!" << std::endl
                         << " [Line " << mNumberOfLines << " ]";
        }

        KRATOS_CATCH("")
    }

    template<class TVariableType>
    void ReadConditionalScalarVariableData(ConditionsContainerType& rThisConditions, TVariableType& rVariable)
    {
        KRATOS_TRY

        SizeType id;
        double conditional_value;

        std::string value;

        while (!mInput.eof())
        {
            ReadWord(value); // reading id
            if (CheckEndBlock("ConditionalData", value))
            {
                break;
            }

            ExtractValue(value, id);

            // readidng nodal_value
            ReadWord(value);
            ExtractValue(value, conditional_value);

            ModelPart::ConditionIterator i_result = rThisConditions.find(id);
            if (i_result != rThisConditions.end())
            {
                i_result->GetValue(rVariable) =  conditional_value;
            }
            //hbui: turn this off for not flush out the screen when running trilinos application
//            else
//                std::cout  << "WARNING! Assigning " << rVariable.Name() << " to not existing condition #" << id << " [Line " << mNumberOfLines << " ]" << std::endl;
        }

        KRATOS_CATCH("")
    }

    template<class TVariableType, class TDataType>
    void ReadConditionalVectorialVariableData(ConditionsContainerType& rThisConditions, TVariableType& rVariable, TDataType Dummy)
    {
        KRATOS_TRY

        SizeType id;
        TDataType conditional_value;

        std::string value;

        while (!mInput.eof())
        {
            ReadWord(value); // reading id
            if (CheckEndBlock("ConditionalData", value))
            {
                break;
            }

            ExtractValue(value, id);

            // readidng nodal_value
            ReadVectorialValue(conditional_value);
            ExtractValue(value, conditional_value);

            ModelPart::ConditionIterator i_result = rThisConditions.find(id);
            if (i_result != rThisConditions.end())
            {
                i_result->GetValue(rVariable) =  conditional_value;
            }
            //hbui: turn this off for not flush out the screen when running trilinos application
//            else
//                std::cout  << "WARNING! Assigning " << rVariable.Name() << " to not existing condition #" << id << " [Line " << mNumberOfLines << " ]" << std::endl;
        }

        KRATOS_CATCH("")
    }

//    SizeType ReadElementsConnectivitiesBlock(ConnectivitiesContainerType& rThisConnectivities)
//    {
//        KRATOS_TRY
//
//        SizeType id;
//        SizeType node_id;
//        SizeType number_of_connectivities = 0;
//
//
//        std::string word;
//        std::string element_name;
//
//        ReadWord(element_name);
//        if(!KratosComponents<Element>::Has(element_name))
//        {
//            KRATOS_ERROR << "Element " << element_name << " is not registered in Kratos."
//                         << " Please check the spelling of the element name and see if the application containing it is registered corectly."
//                         << " [Line " << mNumberOfLines << " ]";
//        }
//
//        Element const& r_clone_element = KratosComponents<Element>::Get(element_name);
//        SizeType number_of_nodes = r_clone_element.GetGeometry().size();
//        ConnectivitiesContainerType::value_type temp_element_nodes;
//
//        while(!mInput.eof())
//        {
//            ReadWord(word); // Reading the element id or End
//            if(CheckEndBlock("Elements", word))
//                break;
//
//            ExtractValue(word,id);
//            ReadWord(word); // Reading the properties id;
//            temp_element_nodes.clear();
//            for(SizeType i = 0 ; i < number_of_nodes ; i++)
//            {
//                ReadWord(word); // Reading the node id;
//                ExtractValue(word, node_id);
//                temp_element_nodes.push_back(node_id);
//            }
//            const int index = id - 1;
//            const int size = rThisConnectivities.size();
//            if(index == size)  // I do push back instead of resizing to size+1
//                rThisConnectivities.push_back(temp_element_nodes);
//            else if(index < size)
//                rThisConnectivities[index]= temp_element_nodes;
//            else
//            {
//                rThisConnectivities.resize(index+1);
//                rThisConnectivities[index]= temp_element_nodes;
//
//            }
//            number_of_connectivities++;
//        }
//
//        return number_of_connectivities;
//
//        KRATOS_CATCH("")
//    }

    SizeType ReadElementsConnectivitiesBlock(ConnectivitiesContainerType& rThisConnectivities)
    {
        KRATOS_TRY

        SizeType id;
        SizeType node_id;
        SizeType number_of_connectivities = 0;

        std::string word;
        std::string element_name;

        ReadWord(element_name);
        if (!KratosComponents<Element>::Has(element_name))
        {
            KRATOS_ERROR << "Element " << element_name << " is not registered in Kratos."
                         << " Please check the spelling of the element name and see if the application containing it is registered corectly."
                         << " [Line " << mNumberOfLines << " ]";
        }

        Element const& r_clone_element = KratosComponents<Element>::Get(element_name);
        SizeType number_of_nodes = r_clone_element.GetGeometry().size();
        ConnectivitiesContainerType::value_type temp_node_ids;

        int CurrentNumberOfLine = mNumberOfLines;
        while (!mInput.eof())
        {
            ReadWord(word); // Reading the element id or End
            if (CheckEndBlock("Elements", word))
            {
                break;
            }

            ExtractValue(word, id);
            ReadWord(word); // Reading the properties id;

            temp_node_ids.clear();

            while (mNumberOfLines == CurrentNumberOfLine)
            {
                ReadWord2(word); // Reading the node id
                if (!word.empty())
                {
                    ExtractValue(word, node_id);
                    temp_node_ids.push_back(node_id);
                }
            }

            const int index = id - 1;
            const int size = rThisConnectivities.size();
            if (index == size) // I do push back instead of resizing to size+1
            {
                rThisConnectivities.push_back(temp_node_ids);
            }
            else if (index < size)
            {
                rThisConnectivities[index] = temp_node_ids;
            }
            else
            {
                rThisConnectivities.resize(index + 1);
                rThisConnectivities[index] = temp_node_ids;
            }
            number_of_connectivities++;

            CurrentNumberOfLine = mNumberOfLines;
        }

        return number_of_connectivities;

        KRATOS_CATCH("")
    }

    SizeType ReadConditionsConnectivitiesBlock(ConnectivitiesContainerType& rThisConnectivities)
    {
        KRATOS_TRY

        SizeType id;
        SizeType node_id;
        SizeType number_of_connectivities = 0;

        std::string word;
        std::string condition_name;

        ReadWord(condition_name);
        if (!KratosComponents<Condition>::Has(condition_name))
        {
            KRATOS_ERROR << "Condition " << condition_name << " is not registered in Kratos."
                         << " Please check the spelling of the condition name and see if the application containing it is registered corectly."
                         << " [Line " << mNumberOfLines << " ]";
        }

        Condition const& r_clone_condition = KratosComponents<Condition>::Get(condition_name);
        SizeType number_of_nodes = r_clone_condition.GetGeometry().size();
        ConnectivitiesContainerType::value_type temp_condition_nodes;

        while (!mInput.eof())
        {
            ReadWord(word); // Reading the condition id or End
            if (CheckEndBlock("Conditions", word))
            {
                break;
            }

            ExtractValue(word, id);
            ReadWord(word); // Reading the properties id;
            temp_condition_nodes.clear();
            for (SizeType i = 0 ; i < number_of_nodes ; i++)
            {
                ReadWord(word); // Reading the node id;
                ExtractValue(word, node_id);
                temp_condition_nodes.push_back(node_id);
            }

            rThisConnectivities.push_back(temp_condition_nodes);
            number_of_connectivities++;
        }

        return number_of_connectivities;

        KRATOS_CATCH("")
    }

    void FillNodalConnectivitiesFromElementBlock(ConnectivitiesContainerType& rNodalConnectivities)
    {
        KRATOS_TRY;

        SizeType id;
        SizeType node_id;
        SizeType number_of_nodes = rNodalConnectivities.size();

        std::string word;
        std::string element_name;

        ReadWord(element_name);
        if (!KratosComponents<Element>::Has(element_name))
        {
            KRATOS_ERROR << "Element " << element_name << " is not registered in Kratos."
                         << " Please check the spelling of the element name and see if the application containing it is registered corectly."
                         << " [Line " << mNumberOfLines << " ]";
        }

        Element const& r_clone_element = KratosComponents<Element>::Get(element_name);
        SizeType n_nodes_in_elem = r_clone_element.GetGeometry().size();
        ConnectivitiesContainerType::value_type temp_element_nodes;

        while (!mInput.eof())
        {
            ReadWord(word); // Reading the element id or End
            if (CheckEndBlock("Elements", word))
            {
                break;
            }

            ExtractValue(word, id);
            ReadWord(word); // Reading the properties id;
            temp_element_nodes.clear();
            for (SizeType i = 0 ; i < n_nodes_in_elem ; i++)
            {
                ReadWord(word); // Reading the node id;
                ExtractValue(word, node_id);
                temp_element_nodes.push_back(node_id);
            }

            for (SizeType i = 0; i < n_nodes_in_elem; i++)
            {
                node_id = temp_element_nodes[i];
                if (node_id > number_of_nodes) // Ids begin on 1
                {
                    KRATOS_ERROR << "Element connectivities contain undefined node with id " << node_id;
                }
                for (SizeType j = 0; j < i; j++)
                {
                    rNodalConnectivities[node_id - 1].push_back(temp_element_nodes[j]);
                }
                for (SizeType j = i + 1; j < n_nodes_in_elem; j++)
                {
                    rNodalConnectivities[node_id - 1].push_back(temp_element_nodes[j]);
                }
            }
        }

        KRATOS_CATCH("");
    }

    void FillNodalConnectivitiesFromConditionBlock(ConnectivitiesContainerType& rNodalConnectivities)
    {
        KRATOS_TRY;

        SizeType id;
        SizeType node_id;
        SizeType number_of_nodes = rNodalConnectivities.size();

        std::string word;
        std::string condition_name;

        ReadWord(condition_name);
        if (!KratosComponents<Condition>::Has(condition_name))
        {
            KRATOS_ERROR << "Condition " << condition_name << " is not registered in Kratos."
                         << " Please check the spelling of the condition name and see if the application containing it is registered corectly."
                         << " [Line " << mNumberOfLines << " ]";
        }

        Condition const& r_clone_condition = KratosComponents<Condition>::Get(condition_name);
        SizeType n_nodes_in_cond = r_clone_condition.GetGeometry().size();
        ConnectivitiesContainerType::value_type temp_condition_nodes;

        while (!mInput.eof())
        {
            ReadWord(word); // Reading the condition id or End
            if (CheckEndBlock("Conditions", word))
            {
                break;
            }

            ExtractValue(word, id);
            ReadWord(word); // Reading the properties id;
            temp_condition_nodes.clear();
            for (SizeType i = 0 ; i < n_nodes_in_cond ; i++)
            {
                ReadWord(word); // Reading the node id;
                ExtractValue(word, node_id);
                temp_condition_nodes.push_back(node_id);
            }

            for (SizeType i = 0; i < n_nodes_in_cond; i++)
            {
                node_id = temp_condition_nodes[i];
                if (node_id > number_of_nodes) // Ids begin on 1
                {
                    KRATOS_ERROR << "Condition connectivities contain undefined node with id " << node_id;
                }
                for (SizeType j = 0; j < i; j++)
                {
                    rNodalConnectivities[node_id - 1].push_back(temp_condition_nodes[j]);
                }
                for (SizeType j = i + 1; j < n_nodes_in_cond; j++)
                {
                    rNodalConnectivities[node_id - 1].push_back(temp_condition_nodes[j]);
                }
            }
        }

        KRATOS_CATCH("");
    }

    void ReadCommunicatorDataBlock(CommunicatorType& rThisCommunicator, NodesContainerType& rThisNodes)
    {
        KRATOS_TRY

//  KRATOS_WATCH("begin reading CommunicatorDataBlock")

        std::string word;
        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }
            if (CheckEndBlock("CommunicatorData", word))
            {
                break;
            }
            if (word == "NEIGHBOURS_INDICES")
            {
                ReadVectorialValue(rThisCommunicator.NeighbourIndices());
            }
            else if (word == "NUMBER_OF_COLORS")
            {
                ReadWord(word);
                SizeType number_of_colors;
                ExtractValue(word, number_of_colors);
                rThisCommunicator.SetNumberOfColors(number_of_colors);
            }
            else
            {
                ReadBlockName(word);
                if (word == "LocalNodes")
                {
                    ReadCommunicatorLocalNodesBlock(rThisCommunicator, rThisNodes);
                }
                else if (word == "GhostNodes")
                {
                    ReadCommunicatorGhostNodesBlock(rThisCommunicator, rThisNodes);
                }
                else
                {
                    SkipBlock(word);
                }
            }
        }

// 	KRATOS_WATCH("finished reading CommunicatorDataBlock")

        return ;

        KRATOS_CATCH("")
    }

    void ReadCommunicatorLocalNodesBlock(CommunicatorType& rThisCommunicator, NodesContainerType& rThisNodes)
    {
        KRATOS_TRY

// 	KRATOS_WATCH("begin reading CommunicatorLocalNodesBlock")

        SizeType interface_id;
        SizeType node_id;

        std::string word;
        std::string condition_name;

        ReadWord(word); // reading the interface id
        ExtractValue(word, interface_id);

        if (interface_id > rThisCommunicator.GetNumberOfColors())
        {
            KRATOS_ERROR << "Interface " << interface_id << " is not valid."
                         << " The number of colors is " << rThisCommunicator.GetNumberOfColors() << " and the interface id must be les than or equal to number of colors"
                         << " [Line " << mNumberOfLines << " ]";
        }

        CommunicatorType::MeshType* p_local_mesh;
        CommunicatorType::MeshType* p_interface_mesh;

        if (interface_id == 0)
        {
            p_local_mesh = &(rThisCommunicator.LocalMesh());
            p_interface_mesh = &(rThisCommunicator.InterfaceMesh());
        }
        else
        {
            p_local_mesh = &(rThisCommunicator.LocalMesh(interface_id - 1));
            p_interface_mesh = &(rThisCommunicator.InterfaceMesh(interface_id - 1));
        }

        while (!mInput.eof())
        {
            ReadWord(word); // Reading the node id or End
            if (CheckEndBlock("LocalNodes", word))
            {
                break;
            }

            ExtractValue(word, node_id);
            NodesContainerType::iterator i_node = FindKey(rThisNodes, node_id, "Node");
            p_local_mesh->Nodes().push_back(*(i_node.base()));
            p_interface_mesh->Nodes().push_back(*(i_node.base()));
        }

        p_local_mesh->Nodes().Sort();
        p_interface_mesh->Nodes().Sort();
// 	KRATOS_WATCH("finished reading CommunicatorLocalNodesBlock")

// 	KRATOS_WATCH(rThisCommunicator)
        KRATOS_CATCH("")
    }

    void ReadCommunicatorGhostNodesBlock(CommunicatorType& rThisCommunicator, NodesContainerType& rThisNodes)
    {
        KRATOS_TRY

// 	KRATOS_WATCH("begin reading CommunicatorGhostNodesBlock")

        SizeType interface_id;
        SizeType node_id;

        std::string word;
        std::string condition_name;

        ReadWord(word); // reading the interface id
        ExtractValue(word, interface_id);

        if (interface_id > rThisCommunicator.GetNumberOfColors())
        {
            KRATOS_ERROR << "Interface " << interface_id << " is not valid."
                         << " The number of colors is " << rThisCommunicator.GetNumberOfColors() << " and the interface id must be les than or equal to number of colors"
                         << " [Line " << mNumberOfLines << " ]";
        }

        CommunicatorType::MeshType* p_ghost_mesh;
        CommunicatorType::MeshType* p_interface_mesh;

        if (interface_id == 0)
        {
            p_ghost_mesh = &(rThisCommunicator.GhostMesh());
            p_interface_mesh = &(rThisCommunicator.InterfaceMesh());
        }
        else
        {
            p_ghost_mesh = &(rThisCommunicator.GhostMesh(interface_id - 1));
            p_interface_mesh = &(rThisCommunicator.InterfaceMesh(interface_id - 1));
        }

        while (!mInput.eof())
        {
            ReadWord(word); // Reading the node id or End
            if (CheckEndBlock("GhostNodes", word))
            {
                break;
            }

            ExtractValue(word, node_id);
            NodesContainerType::iterator i_node = FindKey(rThisNodes, node_id, "Node");
            p_ghost_mesh->Nodes().push_back(*(i_node.base()));
            p_interface_mesh->Nodes().push_back(*(i_node.base()));
        }

        p_ghost_mesh->Nodes().Sort();
        p_interface_mesh->Nodes().Sort();

// 	KRATOS_WATCH(rThisCommunicator)
        KRATOS_CATCH("")

// 	KRATOS_WATCH("finished reading CommunicatorGhostNodesBlock")

    }

    void ReadMeshBlock(ModelPart& rModelPart)
    {
        KRATOS_TRY

// 	KRATOS_WATCH("begin reading CommunicatorDataBlock")

        std::string word;
        SizeType mesh_id;

        ReadWord(word);
        ExtractValue(word, mesh_id);

        KRATOS_WATCH(mesh_id)

        SizeType number_of_meshes = rModelPart.NumberOfMeshes();

        if (mesh_id > 1000000) // this would be a case of error in reading.
        {
            KRATOS_ERROR << "Too large mesh id " << mesh_id;
        }

        if (mesh_id == 0) // this would be a case of error in reading.
        {
            KRATOS_ERROR << "The mesh zero is the reference mesh and already created. You cannot create a mesh 0 with mesh block.";
        }

        // adding necessary meshes to the model part.
        MeshType empty_mesh;
        for (SizeType i = number_of_meshes ; i < mesh_id + 1 ; i++)
#if defined(KRATOS_SD_REF_NUMBER_2)
            rModelPart.GetMeshes().push_back(empty_mesh.Clone());
#elif defined(KRATOS_SD_REF_NUMBER_3)
            rModelPart.GetMeshes().push_back(iga::make_shared<MeshType>(empty_mesh.Clone()));
#endif

        MeshType& mesh = rModelPart.GetMesh(mesh_id);

        while (true)
        {
            ReadWord(word);
            if (mInput.eof())
            {
                break;
            }

            if (CheckEndBlock("Mesh", word))
            {
                break;
            }

            ReadBlockName(word);
            if (word == "MeshNodes")
            {
                ReadMeshNodesBlock(rModelPart, mesh);
            }
            else if (word == "MeshElements")
            {
                ReadMeshElementsBlock(rModelPart, mesh);
            }
            else if (word == "MeshConditions")
            {
                ReadMeshConditionsBlock(rModelPart, mesh);
            }
            else
            {
                SkipBlock(word);
            }
        }

        KRATOS_CATCH("")

    }

    void ReadMeshNodesBlock(ModelPart& rModelPart, MeshType& rMesh)
    {
        KRATOS_TRY

        SizeType node_id;

        std::string word;

        while (!mInput.eof())
        {
            ReadWord(word); // Reading the node id or End
            if (CheckEndBlock("MeshNodes", word))
            {
                break;
            }

            ExtractValue(word, node_id);
            NodesContainerType::iterator i_node = FindKey(rModelPart.Nodes(), node_id, "Node");
            rMesh.Nodes().push_back(*(i_node.base()));
        }

        rMesh.Nodes().Sort();
        KRATOS_CATCH("")
    }

    void ReadMeshElementsBlock(ModelPart& rModelPart, MeshType& rMesh)
    {
        KRATOS_TRY

        SizeType element_id;

        std::string word;

        while (!mInput.eof())
        {
            ReadWord(word); // Reading the element id or End
            if (CheckEndBlock("MeshElements", word))
            {
                break;
            }

            ExtractValue(word, element_id);
            ElementsContainerType::iterator i_element = FindKey(rModelPart.Elements(), element_id, "Element");
            rMesh.Elements().push_back(*(i_element.base()));
        }

        rMesh.Elements().Sort();
        KRATOS_CATCH("")
    }

    void ReadMeshConditionsBlock(ModelPart& rModelPart, MeshType& rMesh)
    {
        KRATOS_TRY

        SizeType condition_id;

        std::string word;

        while (!mInput.eof())
        {
            ReadWord(word); // Reading the element id or End
            if (CheckEndBlock("MeshConditions", word))
            {
                break;
            }

            ExtractValue(word, condition_id);
            ConditionsContainerType::iterator i_condition = FindKey(rModelPart.Conditions(), condition_id, "Condition");
            rMesh.Conditions().push_back(*(i_condition.base()));
        }

        rMesh.Conditions().Sort();
        KRATOS_CATCH("")
    }

    void DivideModelPartDataBlock(OutputFilesContainerType& OutputFiles)
    {
        KRATOS_TRY
        std::string block;

        WriteInAllFiles(OutputFiles, "Begin ModelPartData");

        ReadBlock(block, "ModelPartData");
        WriteInAllFiles(OutputFiles, block);

        WriteInAllFiles(OutputFiles, "End ModelPartData\n");
        KRATOS_WATCH("DivideModelPartDataBlock completed");
        KRATOS_CATCH("")
    }

    void DivideTableBlock(OutputFilesContainerType& OutputFiles)
    {
        KRATOS_TRY

        std::string block;

        WriteInAllFiles(OutputFiles, "Begin Table ");

        ReadBlock(block, "Table");
        WriteInAllFiles(OutputFiles, block);

        WriteInAllFiles(OutputFiles, "End Table\n");
        KRATOS_WATCH("DivideTableBlock completed");
        KRATOS_CATCH("")
    }

    void DividePropertiesBlock(OutputFilesContainerType& OutputFiles)
    {
        KRATOS_TRY

        std::string block;

        WriteInAllFiles(OutputFiles, "Begin Properties ");

        ReadBlock(block, "Properties");
        WriteInAllFiles(OutputFiles, block);

        WriteInAllFiles(OutputFiles, "End Properties\n");
        KRATOS_WATCH("DividePropertiesBlock completed");
        KRATOS_CATCH("")
    }

    void DivideNodesBlock(OutputFilesContainerType& OutputFiles,
                          PartitionIndicesContainerType const& NodesAllPartitions)
    {
        KRATOS_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin Nodes \n");

        SizeType id;

        while (!mInput.eof())
        {
            ReadWord(word);
            if (CheckEndBlock("Nodes", word))
            {
                break;
            }

            ExtractValue(word, id);

            if (id > NodesAllPartitions.size())
            {
                KRATOS_ERROR << "Invalid node id : " << id
                             << " [Line " << mNumberOfLines << " ]";
            }

            std::string node_data;
            node_data += word + '\t'; // id
            ReadWord(word);
            node_data += word + '\t'; // x
            ReadWord(word);
            node_data += word + '\t'; // y
            ReadWord(word);
            node_data += word + '\n'; // z

            for (SizeType i = 0 ; i < NodesAllPartitions[id - 1].size() ; i++)
            {
                SizeType partition_id = NodesAllPartitions[id - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    KRATOS_ERROR << "Invalid partition id : " << partition_id
                                 << " for node " << id << " [Line " << mNumberOfLines << " ]";
                }

                *(OutputFiles[partition_id]) << node_data;
            }

        }

        WriteInAllFiles(OutputFiles, "End Nodes\n");
        KRATOS_WATCH("DivideNodesBlock completed");

        KRATOS_CATCH("")
    }

    void DivideElementsBlock(OutputFilesContainerType& OutputFiles,
                             PartitionIndicesContainerType const& ElementsAllPartitions)
    {
        KRATOS_TRY

        KRATOS_WATCH("DivideElementsBlock started");

        std::string word;
        std::string element_name;

        ReadWord(element_name);
        if (!KratosComponents<Element>::Has(element_name))
        {
            KRATOS_ERROR << "Element " << element_name << " is not registered in Kratos."
                         << " Please check the spelling of the element name and see if the application containing it is registered corectly."
                         << " [Line " << mNumberOfLines << " ]";
        }

        Element const& r_clone_element = KratosComponents<Element>::Get(element_name);
        SizeType number_of_nodes = r_clone_element.GetGeometry().size();

        WriteInAllFiles(OutputFiles, "Begin Elements " +  element_name);

        SizeType id;

        while (!mInput.eof())
        {
            ReadWord(word); // Reading the element id or End
            if (CheckEndBlock("Elements", word))
            {
                break;
            }

            ExtractValue(word, id);
            if (id > ElementsAllPartitions.size())
            {
                KRATOS_ERROR << "Invalid element id : " << id
                             << " [Line " << mNumberOfLines << " ]";
            }

            std::string element_data;
            element_data += '\n' + word + '\t'; // id
            ReadWord(word); // Reading the properties id;
            element_data += word + '\t'; // properties id

            for (SizeType i = 0 ; i < number_of_nodes ; i++)
            {
                ReadWord(word); // Reading the node id;
                element_data += word + '\t'; // node id
            }

            for (SizeType i = 0 ; i < ElementsAllPartitions[id - 1].size() ; i++)
            {
                SizeType partition_id = ElementsAllPartitions[id - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    KRATOS_ERROR << "Invalid partition id : " << partition_id
                                 << " for node " << id << " [Line " << mNumberOfLines << " ]";
                }

                *(OutputFiles[partition_id]) << element_data;
            }

        }

        WriteInAllFiles(OutputFiles, "\nEnd Elements\n");

        KRATOS_WATCH("DivideElementsBlock completed");

        KRATOS_CATCH("")
    }

    void DivideConditionsBlock(OutputFilesContainerType& OutputFiles,
                               PartitionIndicesContainerType const& ConditionsAllPartitions)
    {
        KRATOS_TRY

        std::string word;
        std::string condition_name;

        ReadWord(condition_name);
        if (!KratosComponents<Condition>::Has(condition_name))
        {
            KRATOS_ERROR << "Condition " << condition_name << " is not registered in Kratos."
                         << " Please check the spelling of the condition name and see if the application containing it is registered corectly."
                         << " [Line " << mNumberOfLines << " ]";
        }

        Condition const& r_clone_condition = KratosComponents<Condition>::Get(condition_name);
        SizeType number_of_nodes = r_clone_condition.GetGeometry().size();

        WriteInAllFiles(OutputFiles, "Begin Conditions " +  condition_name);

        SizeType id;

        while (!mInput.eof())
        {
            ReadWord(word); // Reading the condition id or End
            if (CheckEndBlock("Conditions", word))
            {
                break;
            }

            ExtractValue(word, id);
            if (id > ConditionsAllPartitions.size())
            {
                KRATOS_ERROR << "Invalid condition id : " << id
                             << " [Line " << mNumberOfLines << " ]";
            }

            std::string condition_data;
            condition_data += '\n' + word + '\t'; // id
            ReadWord(word); // Reading the properties id;
            condition_data += word + '\t'; // properties id

            for (SizeType i = 0 ; i < number_of_nodes ; i++)
            {
                ReadWord(word); // Reading the node id;
                condition_data += word + '\t'; // node id
            }

            for (SizeType i = 0 ; i < ConditionsAllPartitions[id - 1].size() ; i++)
            {
                SizeType partition_id = ConditionsAllPartitions[id - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    KRATOS_ERROR << "Invalid partition id : " << partition_id
                                 << " for node " << id << " [Line " << mNumberOfLines << " ]";
                }

                *(OutputFiles[partition_id]) << condition_data;
            }

        }

        WriteInAllFiles(OutputFiles, "\nEnd Conditions\n");

        KRATOS_WATCH("DivideConditionsBlock completed");

        KRATOS_CATCH("")
    }

    void DivideNodalDataBlock(OutputFilesContainerType& OutputFiles,
                              PartitionIndicesContainerType const& NodesAllPartitions)
    {
        KRATOS_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin NodalData ");

        std::string variable_name;

        ReadWord(variable_name);

        WriteInAllFiles(OutputFiles, variable_name);
        WriteInAllFiles(OutputFiles, "\n");

        if (KratosComponents<Variable<double> >::Has(variable_name))
        {
            DivideDofVariableData(OutputFiles, NodesAllPartitions);
        }
        else if (KratosComponents<Variable<int> >::Has(variable_name))
        {
            DivideDofVariableData(OutputFiles, NodesAllPartitions);
        }
        else if (KratosComponents<Variable<bool> >::Has(variable_name))
        {
            DivideDofVariableData(OutputFiles, NodesAllPartitions);
        }
#ifndef SD_APP_FORWARD_COMPATIBILITY
        else if (KratosComponents<array_1d_component_type>::Has(variable_name))
        {
            DivideDofVariableData(OutputFiles, NodesAllPartitions);
        }
#endif
        else if (KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
        {
            DivideVectorialVariableData(OutputFiles, NodesAllPartitions, "NodalData");
        }
        else if (KratosComponents<Variable<Matrix> >::Has(variable_name))
        {
            DivideVectorialVariableData(OutputFiles, NodesAllPartitions, "NodalData" );
        }
        else if (KratosComponents<VariableData>::Has(variable_name))
        {
            KRATOS_ERROR << variable_name << " is not supported to be read by this IO or the type of variable is not registered correctly" << std::endl
                         << " [Line " << mNumberOfLines << " ]";
        }
        else
        {
            KRATOS_ERROR << variable_name << " is not a valid variable!!!" << std::endl
                         << " [Line " << mNumberOfLines << " ]";
        }

        WriteInAllFiles(OutputFiles, "End NodalData\n");

        KRATOS_WATCH("DivideNodalDataBlock completed");

        KRATOS_CATCH("")
    }

    void DivideDofVariableData(OutputFilesContainerType& OutputFiles,
                               PartitionIndicesContainerType const& NodesAllPartitions)
    {
        KRATOS_TRY

        SizeType id;

        std::string word;

        while (!mInput.eof())
        {
            ReadWord(word); // reading id
            if (CheckEndBlock("NodalData", word))
            {
                break;
            }

            ExtractValue(word, id);

            if (id > NodesAllPartitions.size())
            {
                KRATOS_ERROR << "Invalid node id : " << id
                             << " [Line " << mNumberOfLines << " ]";
            }

            std::string node_data;
            node_data += word + '\t'; // id
            ReadWord(word);
            node_data += word + '\t'; // is fixed
            ReadWord(word);
            node_data += word + '\n'; // value

            for (SizeType i = 0 ; i < NodesAllPartitions[id - 1].size() ; i++)
            {
                SizeType partition_id = NodesAllPartitions[id - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    KRATOS_ERROR << "Invalid partition id : " << partition_id
                                 << " for node " << id << " [Line " << mNumberOfLines << " ]";
                }

                *(OutputFiles[partition_id]) << node_data;
            }
        }

        KRATOS_CATCH("")
    }

    void DivideVectorialVariableData(OutputFilesContainerType& OutputFiles,
                                     PartitionIndicesContainerType const& EntitiesPartitions,
                                     std::string BlockName)
    {
        KRATOS_TRY

        SizeType id;

        std::string word;

        while (!mInput.eof())
        {
            ReadWord(word); // reading id
            if (CheckEndBlock("NodalData", word))
            {
                break;
            }

            ExtractValue(word, id);

            if (id > EntitiesPartitions.size())
            {
                KRATOS_ERROR << "Invalid node id : " << id
                             << " [Line " << mNumberOfLines << " ]";
            }

            std::string entity_data;
            entity_data += word + '\t'; // id
            Vector temp_vector;
            ReadVectorialValue(temp_vector);

            for (SizeType i = 0 ; i < EntitiesPartitions[id - 1].size() ; i++)
            {
                SizeType partition_id = EntitiesPartitions[id - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    KRATOS_ERROR << "Invalid partition id : " << partition_id
                                 << " for node " << id << " [Line " << mNumberOfLines << " ]";
                }

                *(OutputFiles[partition_id]) << entity_data << temp_vector << std::endl;
            }
        }

        KRATOS_CATCH("")
    }

    void DivideElementalDataBlock(OutputFilesContainerType& OutputFiles,
                                  PartitionIndicesContainerType const& ElementsAllPartitions)
    {
        KRATOS_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin ElementalData ");

        std::string variable_name;

        ReadWord(variable_name);

        WriteInAllFiles(OutputFiles, variable_name);
        WriteInAllFiles(OutputFiles, "\n");

        if (KratosComponents<Variable<double> >::Has(variable_name))
        {
            DivideScalarVariableData(OutputFiles, ElementsAllPartitions, "ElementalData");
        }
        else if (KratosComponents<Variable<bool> >::Has(variable_name))
        {
            DivideScalarVariableData(OutputFiles, ElementsAllPartitions, "ElementalData");
        }
        else if (KratosComponents<Variable<int> >::Has(variable_name))
        {
            DivideScalarVariableData(OutputFiles, ElementsAllPartitions, "ElementalData");
        }
#ifndef SD_APP_FORWARD_COMPATIBILITY
        else if (KratosComponents<array_1d_component_type>::Has(variable_name))
        {
            DivideScalarVariableData(OutputFiles, ElementsAllPartitions, "ElementalData");
        }
#endif
        else if (KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
        {
            DivideVectorialVariableData(OutputFiles, ElementsAllPartitions, "ElementalData");
        }
        else if (KratosComponents<Variable<Matrix> >::Has(variable_name))
        {
            DivideVectorialVariableData(OutputFiles, ElementsAllPartitions, "ElementalData");
        }
        else if (KratosComponents<VariableData>::Has(variable_name))
        {
            KRATOS_ERROR << variable_name << " is not supported to be read by this IO or the type of variable is not registered correctly" << std::endl
                         << " [Line " << mNumberOfLines << " ]";
        }
        else
        {
            KRATOS_ERROR << variable_name << " is not a valid variable!!!" << std::endl
                         << " [Line " << mNumberOfLines << " ]";
        }

        WriteInAllFiles(OutputFiles, "End ElementalData\n");

        KRATOS_CATCH("")
    }

    void DivideScalarVariableData(OutputFilesContainerType& OutputFiles,
                                  PartitionIndicesContainerType const& EntitiesPartitions,
                                  std::string BlockName)
    {
        KRATOS_TRY

        SizeType id;

        std::string word;

        while (!mInput.eof())
        {
            ReadWord(word); // reading id
            if (CheckEndBlock(BlockName, word))
            {
                break;
            }

            ExtractValue(word, id);

            if (id > EntitiesPartitions.size())
            {
                KRATOS_ERROR << "Invalid id : " << id
                             << " [Line " << mNumberOfLines << " ]";
            }

            std::string entity_data;
            entity_data += word + '\t'; // id
            ReadWord(word);
            entity_data += word + '\n'; // value

            for (SizeType i = 0 ; i < EntitiesPartitions[id - 1].size() ; i++)
            {
                SizeType partition_id = EntitiesPartitions[id - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    KRATOS_ERROR << "Invalid partition id : " << partition_id
                                 << " for entity " << id << " [Line " << mNumberOfLines << " ]";
                }

                *(OutputFiles[partition_id]) << entity_data;
            }
        }

        KRATOS_CATCH("")
    }

    void DivideConditionalDataBlock(OutputFilesContainerType& OutputFiles,
                                    PartitionIndicesContainerType const& ConditionsAllPartitions)
    {
        KRATOS_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin ConditionalData ");

        std::string variable_name;

        ReadWord(variable_name);

        WriteInAllFiles(OutputFiles, variable_name);
        WriteInAllFiles(OutputFiles, "\n");

        if (KratosComponents<Variable<double> >::Has(variable_name))
        {
            DivideScalarVariableData(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        }
        else if (KratosComponents<Variable<bool> >::Has(variable_name))
        {
            DivideScalarVariableData(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        }
        else if (KratosComponents<Variable<int> >::Has(variable_name))
        {
            DivideScalarVariableData(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        }
#ifndef SD_APP_FORWARD_COMPATIBILITY
        else if (KratosComponents<array_1d_component_type>::Has(variable_name))
        {
            DivideScalarVariableData(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        }
#endif
        else if (KratosComponents<Variable<array_1d<double, 3> > >::Has(variable_name))
        {
            DivideVectorialVariableData(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        }
        else if (KratosComponents<Variable<Matrix> >::Has(variable_name))
        {
            DivideVectorialVariableData(OutputFiles, ConditionsAllPartitions, "ConditionalData");
        }
        else if (KratosComponents<VariableData>::Has(variable_name))
        {
            KRATOS_ERROR << variable_name << " is not supported to be read by this IO or the type of variable is not registered correctly" << std::endl
                         << " [Line " << mNumberOfLines << " ]";
        }
        else
        {
            KRATOS_ERROR << variable_name << " is not a valid variable!!!" << std::endl
                         << " [Line " << mNumberOfLines << " ]";
        }

        WriteInAllFiles(OutputFiles, "End ConditionalData\n");

        KRATOS_CATCH("")
    }

    void DivideMeshBlock(OutputFilesContainerType& OutputFiles,
                         PartitionIndicesContainerType const& NodesAllPartitions,
                         PartitionIndicesContainerType const& ElementsAllPartitions,
                         PartitionIndicesContainerType const& ConditionsAllPartitions)
    {
        KRATOS_TRY

        std::string word;
        ReadWord(word);

        word += "\n";

        WriteInAllFiles(OutputFiles, "Begin Mesh " + word);

        while (!mInput.eof())
        {
            ReadWord(word);

            if (CheckEndBlock("Mesh", word))
            {
                break;
            }

            ReadBlockName(word);
            if (word == "MeshNodes")
            {
                DivideMeshNodesBlock(OutputFiles, NodesAllPartitions);
            }
            else if (word == "MeshElements")
            {
                DivideMeshElementsBlock(OutputFiles, ElementsAllPartitions);
            }
            else if (word == "MeshConditions")
            {
                DivideMeshConditionsBlock(OutputFiles, ConditionsAllPartitions);
            }
            else
            {
                SkipBlock(word);
            }
        }

        WriteInAllFiles(OutputFiles, "End Mesh\n");

        KRATOS_CATCH("")

    }

    void DivideMeshNodesBlock(OutputFilesContainerType& OutputFiles,
                              PartitionIndicesContainerType const& NodesAllPartitions)
    {
        KRATOS_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin MeshNodes \n");

        SizeType id;

        while (!mInput.eof())
        {
            ReadWord(word);

            if (CheckEndBlock("MeshNodes", word))
            {
                break;
            }

            ExtractValue(word, id);

            if (id > NodesAllPartitions.size())
            {
                KRATOS_ERROR << "Invalid node id : " << id
                             << " [Line " << mNumberOfLines << " ]";
            }

            for (SizeType i = 0 ; i < NodesAllPartitions[id - 1].size() ; i++)
            {
                SizeType partition_id = NodesAllPartitions[id - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    KRATOS_ERROR << "Invalid partition id : " << partition_id
                                 << " for node " << id << " [Line " << mNumberOfLines << " ]";
                }

                *(OutputFiles[partition_id]) << word << std::endl;
            }

        }

        WriteInAllFiles(OutputFiles, "End MeshNodes\n");

        KRATOS_CATCH("")
    }

    void DivideMeshElementsBlock(OutputFilesContainerType& OutputFiles,
                                 PartitionIndicesContainerType const& ElementsAllPartitions)
    {
        KRATOS_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin MeshElements \n");

        SizeType id;

        while (!mInput.eof())
        {
            ReadWord(word);

            if (CheckEndBlock("MeshElements", word))
            {
                break;
            }

            ExtractValue(word, id);

            if (id > ElementsAllPartitions.size())
            {
                KRATOS_ERROR << "Invalid element id : " << id
                             << " [Line " << mNumberOfLines << " ]";
            }

            for (SizeType i = 0 ; i < ElementsAllPartitions[id - 1].size() ; i++)
            {
                SizeType partition_id = ElementsAllPartitions[id - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    KRATOS_ERROR << "Invalid partition id : " << partition_id
                                 << " for element " << id << " [Line " << mNumberOfLines << " ]";
                }

                *(OutputFiles[partition_id]) << word << std::endl;
            }

        }

        WriteInAllFiles(OutputFiles, "End MeshElements\n");

        KRATOS_CATCH("")
    }

    void DivideMeshConditionsBlock(OutputFilesContainerType& OutputFiles,
                                   PartitionIndicesContainerType const& ConditionsAllPartitions)
    {
        KRATOS_TRY

        std::string word;

        WriteInAllFiles(OutputFiles, "Begin MeshConditions \n");

        SizeType id;

        while (!mInput.eof())
        {
            ReadWord(word);

            if (CheckEndBlock("MeshConditions", word))
            {
                break;
            }

            ExtractValue(word, id);

            if (id > ConditionsAllPartitions.size())
            {
                KRATOS_ERROR << "Invalid condition id : " << id
                             << " [Line " << mNumberOfLines << " ]";
            }

            for (SizeType i = 0 ; i < ConditionsAllPartitions[id - 1].size() ; i++)
            {
                SizeType partition_id = ConditionsAllPartitions[id - 1][i];
                if (partition_id > OutputFiles.size())
                {
                    KRATOS_ERROR << "Invalid partition id : " << partition_id
                                 << " for condition " << id << " [Line " << mNumberOfLines << " ]";
                }

                *(OutputFiles[partition_id]) << word << std::endl;
            }

        }

        WriteInAllFiles(OutputFiles, "End MeshConditions\n");

        KRATOS_CATCH("")
    }

    void WritePartitionIndices(OutputFilesContainerType& OutputFiles, PartitionIndicesType const&  NodesPartitions, PartitionIndicesContainerType const& NodesAllPartitions)
    {
        WriteInAllFiles(OutputFiles, "Begin NodalData PARTITION_INDEX\n");

        for (SizeType i_node = 0 ; i_node != NodesAllPartitions.size() ; i_node++)
        {
            for (SizeType i = 0 ; i < NodesAllPartitions[i_node].size() ; i++)
            {
                SizeType partition_id = NodesAllPartitions[i_node][i];
                if (partition_id > OutputFiles.size())
                {
                    KRATOS_ERROR << "Invalid partition id : " << partition_id
                                 << " for node " << i_node + 1 << " [Line " << mNumberOfLines << " ]";
                }

                const SizeType node_partition = NodesPartitions[i_node];
                *(OutputFiles[partition_id]) << i_node + 1 << "  0  " << node_partition << std::endl;
            }
        }

        WriteInAllFiles(OutputFiles, "End NodalData \n");

    }

    void WriteCommunicatorData(OutputFilesContainerType& OutputFiles, SizeType NumberOfPartitions, GraphType const& DomainsColoredGraph,
                               PartitionIndicesType const& NodesPartitions,
                               PartitionIndicesType const& ElementsPartitions,
                               PartitionIndicesType const& ConditionsPartitions,
                               PartitionIndicesContainerType const& NodesAllPartitions,
                               PartitionIndicesContainerType const& ElementsAllPartitions,
                               PartitionIndicesContainerType const& ConditionsAllPartitions)
    {
        WriteInAllFiles(OutputFiles, "Begin CommunicatorData \n");

        // Writing the domains neighbours
        WriteInAllFiles(OutputFiles, "NEIGHBOURS_INDICES    ");
        for (SizeType i_partition = 0 ; i_partition < NumberOfPartitions ; i_partition++)
        {
            boost::numeric::ublas::vector<int> indices = row(DomainsColoredGraph, i_partition);
            *(OutputFiles[i_partition]) << indices << std::endl;
        }

        SizeType number_of_colors = 0;

        for (SizeType i_partition = 0 ; i_partition < DomainsColoredGraph.size1() ; i_partition++)
            for (SizeType i_interface = 0 ; i_interface < DomainsColoredGraph.size2() ; i_interface++)
                if (DomainsColoredGraph(i_partition, i_interface) >= 0)
                    if (number_of_colors < i_interface)
                    {
                        number_of_colors = i_interface;
                    }

        number_of_colors++; // I have to add one to it to get the correct number

        // Writing the max colors
        for (SizeType i_partition = 0 ; i_partition < NumberOfPartitions ; i_partition++)
        {
            boost::numeric::ublas::vector<int> indices = row(DomainsColoredGraph, i_partition);
            *(OutputFiles[i_partition]) << "NUMBER_OF_COLORS    " << number_of_colors << std::endl;
        }

        // Writing the all local nodes
        WriteInAllFiles(OutputFiles, "    Begin LocalNodes 0\n");

        for (SizeType i = 0 ; i < NodesPartitions.size() ; i++)
        {
            *(OutputFiles[NodesPartitions[i]]) << "    " << i + 1 << std::endl;
        }

        WriteInAllFiles(OutputFiles, "    End LocalNodes \n");

        std::vector<PartitionIndicesContainerType> local_nodes_indices(NumberOfPartitions, PartitionIndicesContainerType(NumberOfPartitions));
        std::vector<PartitionIndicesContainerType> ghost_nodes_indices(NumberOfPartitions, PartitionIndicesContainerType(NumberOfPartitions));

        matrix<int> interface_indices = scalar_matrix<int>(NumberOfPartitions, NumberOfPartitions, -1);

        for (SizeType i_partition = 0 ; i_partition < NumberOfPartitions ; i_partition++)
        {
            boost::numeric::ublas::vector<int> neighbours_indices = row(DomainsColoredGraph, i_partition);

            for (SizeType i = 0 ; i <  neighbours_indices.size() ; i++)
                if (SizeType(neighbours_indices[i]) < NumberOfPartitions)
                {
                    interface_indices(i_partition, neighbours_indices[i]) = i;
                }
        }

        for (SizeType i = 0 ; i < NodesPartitions.size() ; i++)
        {
            const SizeType node_partition = NodesPartitions[i];
            const SizeType node_id = i + 1;

            PartitionIndicesType const& node_all_partitions = NodesAllPartitions[i];

            for (SizeType j = 0 ; j < node_all_partitions.size() ; j++)
            {
                SizeType i_node_partition = node_all_partitions[j];
                if (node_partition != i_node_partition)
                {
                    SizeType local_interface_index = interface_indices(node_partition, i_node_partition);
                    SizeType ghost_interface_index = interface_indices(i_node_partition, node_partition);
                    local_nodes_indices[node_partition][local_interface_index].push_back(node_id);
                    ghost_nodes_indices[i_node_partition][ghost_interface_index].push_back(node_id);
                }
            }

        }

        for (SizeType i_partition = 0 ; i_partition < NumberOfPartitions ; i_partition++)
        {
            PartitionIndicesContainerType& partition_local_nodes_indices = local_nodes_indices[i_partition];

            for (SizeType i_interface = 0 ; i_interface < partition_local_nodes_indices.size() ; i_interface++)
            {
                if (partition_local_nodes_indices[i_interface].size() > 0)
                {
                    *(OutputFiles[i_partition]) << "    Begin LocalNodes " << i_interface + 1 << std::endl;
                    for (SizeType i_interface_node = 0 ; i_interface_node < partition_local_nodes_indices[i_interface].size() ; i_interface_node++)
                    {
                        *(OutputFiles[i_partition]) << "    " << partition_local_nodes_indices[i_interface][i_interface_node] << std::endl;
                    }
                    *(OutputFiles[i_partition]) << "    End LocalNodes " << std::endl;
                }
            }

            PartitionIndicesContainerType& partition_ghost_nodes_indices = ghost_nodes_indices[i_partition];

            std::set<unsigned int> all_ghost_nodes_indices;

            for (SizeType i_interface = 0 ; i_interface < partition_ghost_nodes_indices.size() ; i_interface++)
            {
                if (partition_ghost_nodes_indices[i_interface].size() > 0)
                {
                    *(OutputFiles[i_partition]) << "    Begin GhostNodes " << i_interface + 1 << std::endl;
                    for (SizeType i_interface_node = 0 ; i_interface_node < partition_ghost_nodes_indices[i_interface].size() ; i_interface_node++)
                    {
                        *(OutputFiles[i_partition]) << "    " << partition_ghost_nodes_indices[i_interface][i_interface_node] << std::endl;
                        all_ghost_nodes_indices.insert(partition_ghost_nodes_indices[i_interface][i_interface_node]);
                    }
                    *(OutputFiles[i_partition]) << "    End GhostNodes "  << std::endl;
                }
            }

            *(OutputFiles[i_partition]) << "    Begin GhostNodes " << 0 << std::endl;
            for (std::set<unsigned int>::iterator id = all_ghost_nodes_indices.begin() ; id != all_ghost_nodes_indices.end() ; id++)
            {
                *(OutputFiles[i_partition]) << "    " << *id << std::endl;
            }
            *(OutputFiles[i_partition]) << "    End GhostNodes "  << std::endl;

        }

        WriteInAllFiles(OutputFiles, "End CommunicatorData \n");
    }

    void WriteCommunicatorLocalNodes(OutputFilesContainerType& OutputFiles, SizeType NumberOfPartitions, PartitionIndicesType const& NodesPartitions, PartitionIndicesContainerType const& NodesAllPartitions)
    {
        WriteInAllFiles(OutputFiles, "    Begin LocalNodes 0\n");

        for (SizeType i = 0 ; i < NodesPartitions.size() ; i++)
        {
            *(OutputFiles[NodesPartitions[i]]) << "    " << i + 1 << std::endl;
        }

        WriteInAllFiles(OutputFiles, "    End LocalNodes \n");

        PartitionIndicesContainerType local_nodes_indices(NumberOfPartitions);

    }

    void WriteInAllFiles(OutputFilesContainerType& OutputFiles, std::string const& ThisWord)
    {
        for (SizeType i = 0 ; i < OutputFiles.size() ; i++)
        {
            *(OutputFiles[i]) << ThisWord;
        }
    }

    template<class TContainerType, class TKeyType>
    typename TContainerType::iterator FindKey(TContainerType& ThisContainer, TKeyType ThisKey, std::string ComponentName)
    {
        typename TContainerType::iterator i_result;
        if ((i_result = ThisContainer.find(ThisKey)) == ThisContainer.end())
        {
            KRATOS_ERROR << ComponentName << " #" << ThisKey << " is not found."
                         << " [Line " << mNumberOfLines << " ]";
        }

        return i_result;
    }

    // Basically it startes to read the character sequence until reaching a
    // "(" and then goes until corresponding ")" which means the vector or
    // matrix value is completely read. It can be used to read any kind of
    // vector or matrix with operator >> defined and writtern in following
    // format: <some definition> "(" ... ")"
    template<class TValueType>
    TValueType& ReadVectorialValue(TValueType& rValue)
    {
        std::stringstream value;

        char c = SkipWhiteSpaces();
        while ((c != '(') && !mInput.eof())
        {
            value << c;
            c = GetCharacter();
        }
        int open_parantesis = 1;
        while ((open_parantesis != 0) && !mInput.eof())
        {
            value << c;
            c = GetCharacter();
            if (c == '(')
            {
                open_parantesis++;
            }
            if (c == ')')
            {
                open_parantesis--;
            }
        }
        value << c; // adding the final parantesis

        value >>  rValue;

        return rValue;
    }

    template<class TValueType>
    TValueType& ExtractValue(std::string rWord, TValueType & rValue)
    {
        std::stringstream value(rWord);

        value >> rValue;

        return rValue;
    }

    IsogeometricModelPartIO& ReadWord(std::string& Word)
    {
        Word.clear();

        char c = SkipWhiteSpaces();
        while (!mInput.eof() && !IsWhiteSpace(c))
        {
            Word += c;
            c = GetCharacter();
        }

        return *this;
    }

    IsogeometricModelPartIO& ReadWord2(std::string& Word)
    {
        Word.clear();

        char c = GetCharacter();
        while (IsWhiteSpace(c))
        {
            c = GetCharacter();
            if (c == '\n')
            {
                return *this;
            }
        }

        while (!mInput.eof() && !IsWhiteSpace(c))
        {
            Word += c;
            c = GetCharacter();
        }

        return *this;
    }

    IsogeometricModelPartIO& ReadBlock(std::string& Block, std::string const& BlockName)
    {
        Block.clear();

        char c = GetCharacter();
        std::string word;

        while (!mInput.eof())
        {
            if (c == 'E')
            {
                word.clear();
                while (!mInput.eof() && !IsWhiteSpace(c))
                {
                    word += c;
                    c = GetCharacter();
                }
                if (CheckEndBlock(BlockName, word))
                {
                    break;
                }
                Block += word;
            }

            Block += c;

            c = GetCharacter();
        }

        return *this;
    }

    char SkipWhiteSpaces()
    {
        char c = GetCharacter();
        while (IsWhiteSpace(c))
        {
            c = GetCharacter();
        }
        return c;
    }

    bool IsWhiteSpace(char C)
    {
        return ((C == ' ') || (C == '\t') || (C == '\r') || (C == '\n'));
    }

    char GetCharacter() //Getting the next character skipping comments
    {
        char c;
        if (mInput.get(c))
        {
            if (c == '\n')
            {
                mNumberOfLines++;
            }
            else if (c == '/') // it may be a comment!
            {
                char next_c = mInput.peek();
                if (next_c == '/') // it's a line comment
                {
                    while ((mInput.get(c)) && (c != '\n')); // so going to the end of line
                    if (!mInput.eof())
                    {
                        mNumberOfLines++;
                    }
                }
                else if (next_c == '*') // it's a block comment
                {
                    while ((mInput.get(c)) && !((c == '*') && (mInput.peek() == '/'))) // so going to the end of block
                        if (c == '\n')
                        {
                            mNumberOfLines++;
                        }
                    mInput.get(c);
                    c = GetCharacter(); // read a new character after comment
                }
            }
        }
        else
        {
            c = 0;
        }

        return c;

    }

    bool CheckStatement(std::string const& rStatement, std::string const& rGivenWord)
    {
        bool result = false;
        if (rGivenWord != rStatement)
        {
            KRATOS_ERROR << "A \"" << rStatement << "\" statement was expected but the given statement was \""
                         << rGivenWord << "\"" << " [Line " << mNumberOfLines << " ]";
        }
        else
        {
            result = true;
        }

        return result;
    }

    void ResetInput()
    {
        mInput.clear();
        mInput.seekg(0, std::ios_base::beg);
        mNumberOfLines = 1;
    }

    ///@}
    ///@name Private  Access
    ///@{

    ///@}
    ///@name Private Inquiry
    ///@{

    ///@}
    ///@name Un accessible methods
    ///@{

    /// Assignment operator.
    IsogeometricModelPartIO& operator=(IsogeometricModelPartIO const& rOther);

    /// Copy constructor.
    IsogeometricModelPartIO(IsogeometricModelPartIO const& rOther);

    ///@}

}; // Class IsogeometricModelPartIO

///@}

///@name Type Definitions
///@{

///@}
///@name Input and output
///@{

///@}

}  // namespace Kratos.

#endif // KRATOS_MODEL_PART_IO_H_INCLUDED  defined
