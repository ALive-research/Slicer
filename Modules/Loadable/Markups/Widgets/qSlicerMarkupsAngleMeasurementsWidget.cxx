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

#include "qSlicerMarkupsAngleMeasurementsWidget.h"
#include "ui_qSlicerMarkupsAngleMeasurementsWidget.h"

// MRML Markups includes
#include <vtkMRMLMarkupsNode.h>
#include <vtkMRMLMarkupsAngleNode.h>

// --------------------------------------------------------------------------
class qSlicerMarkupsAngleMeasurementsWidgetPrivate
: public Ui_qSlicerMarkupsAngleMeasurementsWidget
{
  Q_DECLARE_PUBLIC(qSlicerMarkupsAngleMeasurementsWidget);

protected:
  qSlicerMarkupsAngleMeasurementsWidget* const q_ptr;

public:
  qSlicerMarkupsAngleMeasurementsWidgetPrivate(qSlicerMarkupsAngleMeasurementsWidget& object);
  ~qSlicerMarkupsAngleMeasurementsWidgetPrivate();

  void setupUi(qSlicerWidget* widget);

  virtual void setupUi(qSlicerMarkupsAngleMeasurementsWidget*);

  // Internal node to operate
  vtkWeakPointer<vtkMRMLMarkupsNode> MarkupsNode;
};

// --------------------------------------------------------------------------
qSlicerMarkupsAngleMeasurementsWidgetPrivate::
qSlicerMarkupsAngleMeasurementsWidgetPrivate(qSlicerMarkupsAngleMeasurementsWidget& object)
  : q_ptr(&object)
{

}

// --------------------------------------------------------------------------
qSlicerMarkupsAngleMeasurementsWidgetPrivate::~qSlicerMarkupsAngleMeasurementsWidgetPrivate() = default;

// --------------------------------------------------------------------------
void qSlicerMarkupsAngleMeasurementsWidgetPrivate::setupUi(qSlicerMarkupsAngleMeasurementsWidget* widget)
{
  Q_Q(qSlicerMarkupsAngleMeasurementsWidget);

  this->Ui_qSlicerMarkupsAngleMeasurementsWidget::setupUi(widget);
  this->angleMeasurementModeCollapsibleButton->setVisible(false);

  QObject::connect(this->angleMeasurementModeComboBox, SIGNAL(currentIndexChanged(int)),
    q, SLOT(onAngleMeasurementModeChanged()));
  QObject::connect(this->rotationAxisCoordinatesWidget, SIGNAL(coordinatesChanged(double*)),
    q, SLOT(onRotationAxisChanged()));
}

// --------------------------------------------------------------------------
qSlicerMarkupsAngleMeasurementsWidget::qSlicerMarkupsAngleMeasurementsWidget(QWidget *parent)
: Superclass(parent), d_ptr(new qSlicerMarkupsAngleMeasurementsWidgetPrivate(*this))
{
  this->setup();
}

// --------------------------------------------------------------------------
qSlicerMarkupsAngleMeasurementsWidget::~qSlicerMarkupsAngleMeasurementsWidget() = default;

// --------------------------------------------------------------------------
void qSlicerMarkupsAngleMeasurementsWidget::setMRMLMarkupsNode(vtkMRMLMarkupsNode* markupsNode)
{
  Q_D(qSlicerMarkupsAngleMeasurementsWidget);
  d->MarkupsNode = markupsNode;
}

// --------------------------------------------------------------------------
void qSlicerMarkupsAngleMeasurementsWidget::setup()
{
  Q_D(qSlicerMarkupsAngleMeasurementsWidget);
  d->setupUi(this);
}
// --------------------------------------------------------------------------
void qSlicerMarkupsAngleMeasurementsWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerMarkupsAngleMeasurementsWidget);

  vtkMRMLMarkupsAngleNode* markupsAngleNode = vtkMRMLMarkupsAngleNode::SafeDownCast(d->MarkupsNode);

  d->angleMeasurementModeCollapsibleButton->setVisible(markupsAngleNode != nullptr);

  if (markupsAngleNode)
    {
    double axisVector[3] = {0.0, 0.0, 0.0};
    markupsAngleNode->GetOrientationRotationAxis(axisVector);
    bool wasBlocked = d->rotationAxisCoordinatesWidget->blockSignals(true);
    d->rotationAxisCoordinatesWidget->setCoordinates(axisVector);
    d->rotationAxisCoordinatesWidget->blockSignals(wasBlocked);
    d->rotationAxisCoordinatesWidget->setEnabled(markupsAngleNode->GetAngleMeasurementMode() != vtkMRMLMarkupsAngleNode::Minimal);
    }
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsAngleMeasurementsWidget::onAngleMeasurementModeChanged()
{
  Q_D(qSlicerMarkupsAngleMeasurementsWidget);
  vtkMRMLMarkupsAngleNode* markupsAngleNode = vtkMRMLMarkupsAngleNode::SafeDownCast(d->MarkupsNode);
  if (!markupsAngleNode)
    {
    return;
    }

  markupsAngleNode->SetAngleMeasurementMode(d->angleMeasurementModeComboBox->currentIndex());
}

//-----------------------------------------------------------------------------
void qSlicerMarkupsAngleMeasurementsWidget::onRotationAxisChanged()
{
  Q_D(qSlicerMarkupsAngleMeasurementsWidget);

  vtkMRMLMarkupsAngleNode* markupsAngleNode = vtkMRMLMarkupsAngleNode::SafeDownCast(d->MarkupsNode);
  if (!markupsAngleNode)
    {
    return;
    }
  markupsAngleNode->SetOrientationRotationAxis(const_cast<double*>(d->rotationAxisCoordinatesWidget->coordinates()));
}
