/* Generated by Together */

#include "TaskInterface.h"
#include "BaseTask/TaskPoint.hpp"

GeoVector TaskInterface::get_active_vector(const AIRCRAFT_STATE &ref)
{
  TaskPoint* tp = getActiveTaskPoint();
  if (tp) {
    return tp->get_vector_remaining(ref);
  } else {
    return GeoVector(0.0,0.0);
  }
}

