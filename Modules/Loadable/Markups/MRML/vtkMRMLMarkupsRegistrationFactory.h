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

#ifndef __vtkMRMLMarkupsRegistrationFactory_h
#define __vtkMRMLMarkupsRegistrationFactory_h

#include "vtkSlicerMarkupsModuleMRMLExport.h"

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// STD includes
#include <map>

class vtkMRMLMarkupsNode;
class vtkSlicerMarkupsWidget;

/// \brief Class to register Markups so they can be utilized by different
/// components in the Markups module
///
/// This singleton class keeps the registration of markups
/// Singleton pattern adopted from vtkSegmentationConverterFactory
class VTK_SLICER_MARKUPS_MODULE_MRML_EXPORT vtkMRMLMarkupsRegistrationFactory : public vtkObject
{
public:

  vtkTypeMacro(vtkMRMLMarkupsRegistrationFactory, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

public:
  /// Return the singleton instance with no reference counting.
  static vtkMRMLMarkupsRegistrationFactory* GetInstance();

  /// This is a singleton pattern New.  There will only be ONE
  /// reference to a vtkMRMLMarkupsRegistrationFactory object per process.
  /// Clients that call this must call Delete on the object so that the reference
  /// counting will work. The single instance will be unreferenced when
  /// the program exits.
  static vtkMRMLMarkupsRegistrationFactory* New();

  /// This reigsters a new markup.
  /// \param markupsNode vtkMRMLMarkups node to register.
  /// \param markupsWidget vtkSlicerMarkupsWidget associated to the markup node.
  void RegisterMarkup(vtkMRMLMarkupsNode *markupsNode,
                      vtkSlicerMarkupsWidget*markupsWidget);

  /// This returns an isntance to a corresponding vtkSlicerMarkupsWidget associated
  /// to the MRML node class specified.
  /// \param nodeClass registered class to retrieve the associated widget.
  /// \return pointer to associated vtkSLicerMarkupsWidget or nullptr if the MRML node
  /// class is not registered.
  vtkSlicerMarkupsWidget* GetWidgetByMarkupsNodeClass(const std::string& nodeClass) const;

  /// This checks if the MRML node specified is registered.
  /// \param nodeClass MRML node class name to check.
  /// \return true, if the node is registered in the factory, fasle otherwise.
  bool IsRegistered(const std::string& nodeClass) const;

protected:
  vtkMRMLMarkupsRegistrationFactory();
  ~vtkMRMLMarkupsRegistrationFactory() override;
  vtkMRMLMarkupsRegistrationFactory(const vtkMRMLMarkupsRegistrationFactory&);
  void operator=(const vtkMRMLMarkupsRegistrationFactory&);

  // Singleton management functions.
  static void classInitialize();
  static void classFinalize();

  friend class vtkMRMLMarkupsRegistrationFactoryInitialize;
  typedef vtkMRMLMarkupsRegistrationFactory Self;

  // Keeps track of the registered nodes and corresponding widgets
  std::map<std::string, vtkSmartPointer<vtkSlicerMarkupsWidget>> MarkupsWidgetsMap;
};

/// Utility class to make sure qSlicerModuleManager is initialized before it is used.
class VTK_SLICER_MARKUPS_MODULE_MRML_EXPORT vtkMRMLMarkupsRegistrationFactoryInitialize
{
public:
  typedef vtkMRMLMarkupsRegistrationFactoryInitialize Self;

  vtkMRMLMarkupsRegistrationFactoryInitialize();
  ~vtkMRMLMarkupsRegistrationFactoryInitialize();
private:
  static unsigned int Count;
};

/// This instance will show up in any translation unit that uses
/// vtkMRMLMarkupsRegistrationFactory. It will make sure
/// vtkMRMLMarkupsRegistrationFactory is initialized before it is used.
static vtkMRMLMarkupsRegistrationFactoryInitialize vtkMRMLMarkupsRegistrationFactoryInitializer;

#endif
