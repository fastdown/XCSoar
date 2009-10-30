/* Generated by Together */

#include "SampledTaskPoint.h"
#include "ConvexHull/GrahamScan.hpp"
#include "ConvexHull/PolygonInterior.hpp"

bool SampledTaskPoint::prune_sample_points()
{
  bool changed=false;
  GrahamScan gs(sampled_points);
  sampled_points = gs.prune_interior(&changed);
  return changed;
}

bool SampledTaskPoint::prune_boundary_points()
{
  bool changed=false;
  GrahamScan gs(boundary_points);
  boundary_points = gs.prune_interior(&changed);
  return changed;
}

const SearchPointVector& 
SampledTaskPoint::get_boundary_points() const
{
  return boundary_points;
}

const SearchPointVector& 
SampledTaskPoint::get_search_points(bool cheat)
{
  if (cheat && !sampled_points.size()) {
    // this adds a point in case the waypoint was skipped
    // this is a crude way of handling the situation --- may be best
    // to de-rate the score in some way

    SearchPoint sp(getLocation(), get_task_projection());
    sampled_points.push_back(sp);
    return sampled_points;
  }
  if (sampled_points.size()>0) {
    return sampled_points;
  } else {
    return boundary_points;
  }
}


void SampledTaskPoint::default_boundary_points() { 
  double t=0;
  if (boundary_scored) {
    for (t=0; t<=1.0; t+= 0.05) {
      SearchPoint sp(get_boundary_parametric(t), get_task_projection());
      boundary_points.push_back(sp);
    }
  } else {
    SearchPoint sp(getLocation(), get_task_projection());
    boundary_points.push_back(sp);
  }
}

bool SampledTaskPoint::update_sample(const AIRCRAFT_STATE& state)
{
  if (isInSector(state)) {
    // if sample is inside sample polygon
    //   return false (no update required)
    // else
    //   add sample to polygon
    //   re-compute convex hull
    //   return true; (update required)
    //
    if (PolygonInterior(state.Location, sampled_points)) {
      // do nothing
      return false;
    } else {
      SearchPoint sp(state.Location, get_task_projection(), true);
      sampled_points.push_back(sp);
      // only return true if hull changed 
      return (prune_sample_points());
    }
  }
  return false;
}

void 
SampledTaskPoint::update_projection()
{
  for (unsigned i=0; i<sampled_points.size(); i++) {
    sampled_points[i].project(get_task_projection());
  }
  for (unsigned i=0; i<boundary_points.size(); i++) {
    boundary_points[i].project(get_task_projection());
  }
}

void 
SampledTaskPoint::clear_sample_all_but_last(const AIRCRAFT_STATE& ref_last) 
{
  if (sampled_points.size()) {
    sampled_points.clear();
    update_sample(ref_last); // add last point
  }
}

void
SampledTaskPoint::clear_boundary_points()
{
  boundary_points.clear();
  search_max = SearchPoint(getLocation(), get_task_projection());
  search_min = SearchPoint(getLocation(), get_task_projection());
}

void 
SampledTaskPoint::clear_sample_points() {
  sampled_points.clear();
}

void 
SampledTaskPoint::print_boundary(std::ostream& f, const AIRCRAFT_STATE &state) const
{
  f << "#   Boundary points\n";
  const unsigned n= get_boundary_points().size();
  for (unsigned i=0; i<n; i++) {
    const GEOPOINT loc = get_boundary_points()[i].getLocation();
    f << "     " << loc.Longitude << " " << loc.Latitude << "\n";
  }
  f << "\n";
}

void 
SampledTaskPoint::print(std::ostream& f, const AIRCRAFT_STATE &state) const
{
  TaskPoint::print(f,state);
  print_boundary(f, state);
}

void 
SampledTaskPoint::print_samples(std::ostream& f,
  const AIRCRAFT_STATE &state) 
{
  const unsigned n= get_search_points().size();
  f << "#   Search points\n";
  for (unsigned i=0; i<n; i++) {
    const GEOPOINT loc = get_search_points()[i].getLocation();
    f << "     " << loc.Longitude << " " << loc.Latitude << "\n";
  }
  f << "\n";
}


