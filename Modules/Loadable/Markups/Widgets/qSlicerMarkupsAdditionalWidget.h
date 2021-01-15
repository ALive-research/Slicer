/*==============================================================================

  Copyright (c) The Intervention Centre
  Oslo University Hospital, Oslo, Norway. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Rafael Palomar (The Intervention Centre,
  Oslo University Hospital) and was supported by The Research Council of Norway
  through the ALive project (grant nr. 311393).

==============================================================================*/

#ifndef __qslicermarkupsadditionalwidget_h_
#define __qslicermarkupsadditionalwidget_h_

// Qt Slicer includes
#include "qSlicerWidget.h"

// Markups Widgets includes
#include "qSlicerMarkupsModuleWidgetsExport.h"

class vtkMRMLMarkupsNode;

class Q_SLICER_MODULE_MARKUPS_WIDGETS_EXPORT
qSlicerMarkupsAdditionalWidget : public qSlicerWidget
{
  Q_OBJECT

public:
  typedef qSlicerWidget Superclass;
  qSlicerMarkupsAdditionalWidget(QWidget* parent=nullptr);
  ~qSlicerMarkupsAdditionalWidget()=default;

  /// Sets the vtkMRMLMarkupsNode to operate on.
  virtual void setMRMLMarkupsNode(vtkMRMLMarkupsNode*) = 0;
  /// Updates the widget based on information from MRML.
  virtual void updateWidgetFromMRML() = 0;

private:
  Q_DISABLE_COPY(qSlicerMarkupsAdditionalWidget);
};

#endif // __qslicermarkupsadditionalwidget_h_
