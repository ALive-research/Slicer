/*==============================================================================

  Copyright (c) The Intervention Centre, Oslo University Hospital.
  All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was developed by Rafael Palomar (Oslo University Hospital), based on
  the original vtkSegmentationConverterFactory.h from Andras Lasso
  (Queen's University, Ontario, Canada), and was supported by The Research
  Council of Norway (Grant 311393 -- ALive project).
==============================================================================*/

#include "vtkMRMLMarkupsRegistrationFactory.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkDataObject.h>

// MRML includes
#include "vtkMRMLMarkupsNode.h"
#include "vtkSlicerMarkupsWidget.h"

//----------------------------------------------------------------------------
// This MUST be default initialized to zero by the compiler and is
// therefore not initialized here.  The ClassInitialize and ClassFinalize methods handle this instance.
static vtkMRMLMarkupsRegistrationFactory* vtkMRMLMarkupsRegistrationFactoryInstance;

//----------------------------------------------------------------------------
// Must NOT be initialized.  Default initialization to zero is necessary.
unsigned int vtkMRMLMarkupsRegistrationFactoryInitialize::Count;

//----------------------------------------------------------------------------
// Implementation of vtkMRMLMarkupsRegistrationFactoryInitialize class.
//----------------------------------------------------------------------------
vtkMRMLMarkupsRegistrationFactoryInitialize::vtkMRMLMarkupsRegistrationFactoryInitialize()
{
  if(++Self::Count == 1)
    {
    vtkMRMLMarkupsRegistrationFactory::classInitialize();
    }
}

//----------------------------------------------------------------------------
vtkMRMLMarkupsRegistrationFactoryInitialize::~vtkMRMLMarkupsRegistrationFactoryInitialize()
{
  if(--Self::Count == 0)
    {
    vtkMRMLMarkupsRegistrationFactory::classFinalize();
    }
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Up the reference count so it behaves like New
vtkMRMLMarkupsRegistrationFactory* vtkMRMLMarkupsRegistrationFactory::New()
{
  vtkMRMLMarkupsRegistrationFactory* ret = vtkMRMLMarkupsRegistrationFactory::GetInstance();
  ret->Register(nullptr);
  return ret;
}

//----------------------------------------------------------------------------
// Return the single instance of the vtkMRMLMarkupsRegistrationFactory
vtkMRMLMarkupsRegistrationFactory* vtkMRMLMarkupsRegistrationFactory::GetInstance()
{
  if(!vtkMRMLMarkupsRegistrationFactoryInstance)
    {
    // Try the factory first
    vtkMRMLMarkupsRegistrationFactoryInstance =
      (vtkMRMLMarkupsRegistrationFactory*)vtkObjectFactory::CreateInstance("vtkMRMLMarkupsRegistrationFactory");

    // if the factory did not provide one, then create it here
    if(!vtkMRMLMarkupsRegistrationFactoryInstance)
      {
      vtkMRMLMarkupsRegistrationFactoryInstance = new vtkMRMLMarkupsRegistrationFactory;
#ifdef VTK_HAS_INITIALIZE_OBJECT_BASE
      vtkMRMLMarkupsRegistrationFactoryInstance->InitializeObjectBase();
#endif
      }
    }
  // return the instance
  return vtkMRMLMarkupsRegistrationFactoryInstance;
}

//----------------------------------------------------------------------------
vtkMRMLMarkupsRegistrationFactory::vtkMRMLMarkupsRegistrationFactory() = default;

//----------------------------------------------------------------------------
vtkMRMLMarkupsRegistrationFactory::~vtkMRMLMarkupsRegistrationFactory() = default;

//----------------------------------------------------------------------------
void vtkMRMLMarkupsRegistrationFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsRegistrationFactory::classInitialize()
{
  // Allocate the singleton
  vtkMRMLMarkupsRegistrationFactoryInstance = vtkMRMLMarkupsRegistrationFactory::GetInstance();
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsRegistrationFactory::classFinalize()
{
  vtkMRMLMarkupsRegistrationFactoryInstance->Delete();
  vtkMRMLMarkupsRegistrationFactoryInstance = nullptr;
}

//----------------------------------------------------------------------------
void vtkMRMLMarkupsRegistrationFactory::RegisterMarkup(vtkMRMLMarkupsNode *markupsNode,
                                                       vtkSlicerMarkupsWidget* markupsWidget)
{
  if (markupsNode == nullptr)
    {
    vtkErrorMacro("RegisterMarkup: Invalid node.");
    return;
    }

  if (markupsWidget == nullptr)
    {
    vtkErrorMacro("RegisterMarkup: Invalid widget.");
    return;
    }

  // Take ownership of widget regardless of the outcome of this function (we will
  // manage the memory).
  vtkSmartPointer<vtkMRMLNode> node =
    vtkSmartPointer<vtkMRMLNode>::Take(markupsNode);

  vtkSmartPointer<vtkSlicerMarkupsWidget> associatedWidget =
    vtkSmartPointer<vtkSlicerMarkupsWidget>::Take(markupsWidget);

  // Check that the class is not already registered
  if (this->MarkupsWidgetsMap.count(node->GetClassName()))
    {
    vtkWarningMacro("RegisterMarkup: Markups node " << node->GetClassName()<<
                    " is already registered.");
    return;
    }

  this->MarkupsWidgetsMap[node->GetClassName()] = associatedWidget;
}

//----------------------------------------------------------------------------
vtkSlicerMarkupsWidget* vtkMRMLMarkupsRegistrationFactory::GetWidgetByMarkupsNodeClass(const std::string& nodeClass) const
{
  auto search = this->MarkupsWidgetsMap.find(nodeClass);
  if (search != this->MarkupsWidgetsMap.end())
    {
    return search->second;
    }

  return nullptr;
}

//----------------------------------------------------------------------------
bool vtkMRMLMarkupsRegistrationFactory::IsRegistered(const std::string& nodeClass) const
{
  return (this->MarkupsWidgetsMap.find(nodeClass) != this->MarkupsWidgetsMap.end());
}
