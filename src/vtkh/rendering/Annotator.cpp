#include <vtkh/rendering/Annotator.hpp>

namespace vtkh
{
  
Annotator::Annotator(vtkm::rendering::Canvas &canvas, 
                     vtkm::rendering::Camera &camera,
                     vtkm::Bounds bounds)
  : m_canvas(canvas), 
    m_camera(camera),
    m_bounds(bounds)
{
  m_is_3d = m_camera.GetMode() == vtkm::rendering::Camera::MODE_3D;
  m_world_annotator = m_canvas.CreateWorldAnnotator(); 
}  

Annotator::~Annotator()
{
  delete m_world_annotator;
}

void 
Annotator::RenderScreenAnnotations(const std::string &fieldname, 
                                   vtkm::Range range,
                                   vtkm::rendering::ColorTable color_table)
{
  m_canvas.SetViewToScreenSpace(m_camera, true);
  this->m_color_bar_annotation.SetRange(range, 5);
  this->m_color_bar_annotation.SetColorTable(color_table);
  this->m_color_bar_annotation.Render(m_camera, *m_world_annotator, m_canvas);
}

void Annotator::RenderWorldAnnotations()
{
  if(!m_is_3d) return;
  m_canvas.SetViewToWorldSpace(m_camera, false);
  vtkm::Float64 xmin = m_bounds.X.Min, xmax = m_bounds.X.Max;
  vtkm::Float64 ymin = m_bounds.Y.Min, ymax = m_bounds.Y.Max;
  vtkm::Float64 zmin = m_bounds.Z.Min, zmax = m_bounds.Z.Max;
  vtkm::Float64 dx = xmax - xmin, dy = ymax - ymin, dz = zmax - zmin;
  vtkm::Float64 size = vtkm::Sqrt(dx * dx + dy * dy + dz * dz);

  this->m_box_annotation.SetColor(vtkm::rendering::Color(.5f, .5f, .5f));
  this->m_box_annotation.SetExtents(m_bounds);
  this->m_box_annotation.Render(m_camera, *m_world_annotator);
  vtkm::Vec<vtkm::Float32, 3> lookAt = m_camera.GetLookAt();
  vtkm::Vec<vtkm::Float32, 3> position = m_camera.GetPosition();
  bool xtest = lookAt[0] > position[0];
  bool ytest = lookAt[1] > position[1];
  bool ztest = lookAt[2] > position[2];

  const bool outsideedges = true; // if false, do closesttriad
  if (outsideedges)
  {
    xtest = !xtest;
    //ytest = !ytest;
  }

  vtkm::Float64 xrel = vtkm::Abs(dx) / size;
  vtkm::Float64 yrel = vtkm::Abs(dy) / size;
  vtkm::Float64 zrel = vtkm::Abs(dz) / size;
  vtkm::rendering::Color axis_color(1.f, 1.f, 1.f);
  float major_tick_size = size / 40.f;
  float minor_tick_size = size / 80.f;
  this->m_x_axis_annotation.SetAxis(0);
  this->m_x_axis_annotation.SetColor(axis_color);
  this->m_x_axis_annotation.SetTickInvert(xtest, ytest, ztest);
  this->m_x_axis_annotation.SetWorldPosition(
    xmin, ytest ? ymin : ymax, ztest ? zmin : zmax, xmax, ytest ? ymin : ymax, ztest ? zmin : zmax);
  this->m_x_axis_annotation.SetRange(xmin, xmax);
  this->m_x_axis_annotation.SetMajorTickSize(major_tick_size, 0);
  this->m_x_axis_annotation.SetMinorTickSize(minor_tick_size, 0);
  this->m_x_axis_annotation.SetLabelFontOffset(vtkm::Float32(size / 15.f));
  this->m_x_axis_annotation.SetMoreOrLessTickAdjustment(-1);
  //this->m_x_axis_annotation.SetMoreOrLessTickAdjustment(xrel < .3 ? -1 : 0);
  this->m_x_axis_annotation.Render(m_camera, *m_world_annotator, m_canvas);

  this->m_y_axis_annotation.SetAxis(1);
  this->m_y_axis_annotation.SetColor(axis_color);
  this->m_y_axis_annotation.SetTickInvert(xtest, ytest, ztest);
  this->m_y_axis_annotation.SetWorldPosition(
    xtest ? xmin : xmax, ymin, ztest ? zmin : zmax, xtest ? xmin : xmax, ymax, ztest ? zmin : zmax);
  this->m_y_axis_annotation.SetRange(ymin, ymax);
  this->m_y_axis_annotation.SetMajorTickSize(major_tick_size, 0);
  this->m_y_axis_annotation.SetMinorTickSize(minor_tick_size, 0);
  this->m_y_axis_annotation.SetLabelFontOffset(vtkm::Float32(size / 15.f));
  this->m_y_axis_annotation.SetMoreOrLessTickAdjustment(-1);
  //this->m_y_axis_annotation.SetMoreOrLessTickAdjustment(yrel < .3 ? -1 : 0);
  this->m_y_axis_annotation.Render(m_camera, *m_world_annotator, m_canvas);

  this->m_z_axis_annotation.SetAxis(2);
  this->m_z_axis_annotation.SetColor(axis_color);
  this->m_z_axis_annotation.SetTickInvert(xtest, ytest, ztest);
  this->m_z_axis_annotation.SetWorldPosition(
    xtest ? xmin : xmax, ytest ? ymin : ymax, zmin, xtest ? xmin : xmax, ytest ? ymin : ymax, zmax);
  this->m_z_axis_annotation.SetRange(zmin, zmax);
  this->m_z_axis_annotation.SetMajorTickSize(major_tick_size, 0);
  this->m_z_axis_annotation.SetMinorTickSize(minor_tick_size, 0);
  this->m_z_axis_annotation.SetLabelFontOffset(vtkm::Float32(size / 15.f));
  //this->m_z_axis_annotation.SetMoreOrLessTickAdjustment(zrel < .3 ? -1 : 0);
  this->m_z_axis_annotation.SetMoreOrLessTickAdjustment(-1);
  this->m_z_axis_annotation.Render(m_camera, *m_world_annotator, m_canvas);

}

} //namespace vtkh