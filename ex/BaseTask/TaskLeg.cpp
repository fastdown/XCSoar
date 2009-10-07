/* Generated by Together */

#include "TaskLeg.h"
#include "Util.h"
#include "OrderedTaskPoint.hpp"
#include <assert.h>
#include <algorithm>

double TaskLeg::leg_distance_scored(const GEOPOINT &ref)
{
  switch (tp_destination->getActiveState()) {
  case OrderedTaskPoint::BEFORE_ACTIVE:
    // this leg totally included
    return 
      std::max(0.0,
               ::Distance(tp_origin->get_reference_scored(), 
                          tp_destination->get_reference_scored())
               -tp_origin->score_adjustment()-tp_destination->score_adjustment());
    break;
  case OrderedTaskPoint::AFTER_ACTIVE:
    // this leg not included
    return 0.0;
  case OrderedTaskPoint::CURRENT_ACTIVE:
    // this leg partially included
    return 
      std::max(0.0,
               ::ProjectedDistance(tp_origin->get_reference_scored(), 
                                   tp_destination->get_reference_scored(),
                                   ref)
               -tp_origin->score_adjustment()-tp_destination->score_adjustment());
    break;
  default:
    assert(1); // error!
    break;
  };
  assert(1); // error!
  return 0.0;
}


double TaskLeg::leg_distance_remaining(const GEOPOINT &ref)
{
  switch (tp_destination->getActiveState()) {
  case OrderedTaskPoint::AFTER_ACTIVE:
    // this leg totally included
    return 
      ::Distance(tp_origin->get_reference_remaining(), 
                 tp_destination->get_reference_remaining());
    break;
  case OrderedTaskPoint::BEFORE_ACTIVE:
    // this leg not included
    return 0.0;
  case OrderedTaskPoint::CURRENT_ACTIVE:
    // this leg partially included
    return 
      ::Distance(ref, 
                 tp_destination->get_reference_remaining());
    break;
  default:
    assert(1); // error!
    break;
  };
  assert(1); // error!
  return 0.0;
}


double TaskLeg::leg_distance_travelled(const GEOPOINT &ref)
{
  switch (tp_destination->getActiveState()) {
  case OrderedTaskPoint::BEFORE_ACTIVE:
    // this leg totally included
    return 
      ::Distance(tp_origin->get_reference_travelled(), 
                 tp_destination->get_reference_travelled());
    break;
  case OrderedTaskPoint::AFTER_ACTIVE:
    // this leg not included
    return 0.0;
  case OrderedTaskPoint::CURRENT_ACTIVE:
    // this leg partially included
    if (tp_destination->has_entered()) {
      ::Distance(tp_origin->get_reference_travelled(), 
                 tp_destination->get_reference_travelled())
        +::Distance(tp_destination->get_reference_travelled(), 
                    ref);
    } else {
      return 
        ::Distance(tp_origin->get_reference_travelled(), 
                   ref);
    }
    break;
  default:
    assert(1); // error!
    break;
  };
  assert(1); // error!
  return 0.0;
}

double TaskLeg::leg_distance_nominal()
{
  return 
    ::Distance(tp_origin->get_reference_nominal(), 
               tp_destination->get_reference_nominal());
}


///////

void TaskLeg::update_geometry()
{
  tp_origin->update_geometry();
  tp_destination->update_geometry();
}


TaskLeg::TaskLeg(OrderedTaskPoint& origin,
                 OrderedTaskPoint& destination):
  tp_origin(&origin),
  tp_destination(&destination)
{
  tp_origin->set_leg_out(this);
  tp_destination->set_leg_in(this);
}


OrderedTaskPoint* TaskLeg::get_destination() const {
  return tp_destination;
}

OrderedTaskPoint* TaskLeg::get_origin() const {
  return tp_origin;
}


double TaskLeg::leg_bearing_remaining(const GEOPOINT &ref)
{
  switch (tp_destination->getActiveState()) {
  case OrderedTaskPoint::AFTER_ACTIVE:
    // this leg totally included
    return 
      ::Bearing(tp_origin->get_reference_remaining(), 
                tp_destination->get_reference_remaining());
    break;
  case OrderedTaskPoint::BEFORE_ACTIVE:
    // this leg not included
    return 0.0;
  case OrderedTaskPoint::CURRENT_ACTIVE:
    // this leg partially included
    return 
      ::Bearing(ref, 
                tp_destination->get_reference_remaining());
    break;
  default:
    assert(1); // error!
    break;
  };
  assert(1); // error!
  return 0.0;
}


double TaskLeg::leg_bearing_travelled(const GEOPOINT &ref)
{
  switch (tp_destination->getActiveState()) {
  case OrderedTaskPoint::BEFORE_ACTIVE:
    // this leg totally included
    return 
      ::Bearing(tp_origin->get_reference_travelled(), 
                tp_destination->get_reference_travelled());
    break;
  case OrderedTaskPoint::AFTER_ACTIVE:
    // this leg not included
    return 0.0;
  case OrderedTaskPoint::CURRENT_ACTIVE:
    // this leg partially included
    return 
      ::Bearing(tp_origin->get_reference_travelled(), 
                ref);
    break;
  default:
    assert(1); // error!
    break;
  };
  assert(1); // error!
  return 0.0;
}
