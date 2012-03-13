/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestChartXYZ.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkChartXYZ.h"
#include "vtkContextView.h"
#include "vtkContextScene.h"
#include "vtkFloatArray.h"

#include "vtkRenderer.h"
#include "vtkRenderView.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkNew.h"
#include "vtkTable.h"
#include "vtkCallbackCommand.h"

// Need a timer so that we can animate, and then take a snapshot!
namespace
{
static double angle = 0;

void ProcessEvents(vtkObject *caller, unsigned long,
                   void *clientData, void *callerData)
{
  vtkChartXYZ *chart = reinterpret_cast<vtkChartXYZ *>(clientData);
  vtkRenderWindowInteractor *interactor =
      reinterpret_cast<vtkRenderWindowInteractor *>(caller);
  angle += 2;
  chart->SetAngle(angle);
  interactor->Render();
  if (angle >= 90)
    {
    int timerId = *reinterpret_cast<int *>(callerData);
    interactor->DestroyTimer(timerId);
    }
}
} // End of anonymous namespace.

int TestChartXYZ(int , char * [])
{
  // Now the chart
  vtkNew<vtkChartXYZ> chart;
  vtkNew<vtkContextView> view;
  view->GetRenderWindow()->SetSize(400, 300);
  view->GetScene()->AddItem(chart.GetPointer());
  vtkNew<vtkChartXYZ> chart2;
  view->GetScene()->AddItem(chart2.GetPointer());

  chart->SetGeometry(vtkRectf(75.0, 20.0, 250, 260));
  chart2->SetGeometry(vtkRectf(75.0, 20.0, 250, 260));

  // Create a table with some points in it...
  vtkNew<vtkTable> table;
  vtkNew<vtkFloatArray> arrX;
  arrX->SetName("X Axis");
  table->AddColumn(arrX.GetPointer());
  vtkNew<vtkFloatArray> arrC;
  arrC->SetName("Cosine");
  table->AddColumn(arrC.GetPointer());
  vtkNew<vtkFloatArray> arrS;
  arrS->SetName("Sine");
  table->AddColumn(arrS.GetPointer());
  // Test charting with a few more points...
  int numPoints = 69;
  float inc = 7.5 / (numPoints-1);
  table->SetNumberOfRows(numPoints);
  table->SetNumberOfRows(numPoints);
  for (int i = 0; i < numPoints; ++i)
    {
    table->SetValue(i, 0, i * inc);
    table->SetValue(i, 1, cos(i * inc) + 0.0);
    table->SetValue(i, 2, sin(i * inc) + 0.0);
    }

  //chart->SetAroundX(true);
  // Add the three dimensions we are interested in visualizing.
  chart->SetInput(table.GetPointer(), "X Axis", "Sine", "Cosine");
  chart->RecalculateBounds();
  chart->RecalculateTransform();

  // We want a duplicate, that does not move.
  chart2->SetInput(table.GetPointer(), "X Axis", "Sine", "Cosine");
  chart2->RecalculateBounds();
  chart2->RecalculateTransform();

  view->GetRenderWindow()->SetMultiSamples(0);
  view->GetInteractor()->Initialize();

  // Set up the timer, and be sure to incrememt the angle.
  vtkNew<vtkCallbackCommand> callback;
  callback->SetClientData(chart.GetPointer());
  callback->SetCallback(::ProcessEvents);
  view->GetInteractor()->AddObserver(vtkCommand::TimerEvent, callback.GetPointer(),
                                0);
  view->GetInteractor()->CreateRepeatingTimer(1000 / 25);

  view->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
