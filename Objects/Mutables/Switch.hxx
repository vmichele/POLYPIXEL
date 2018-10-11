#ifndef SWITCH_HXX
#define SWITCH_HXX

#include "Objects/Mutables/Mutable.hxx"

class Switch: public Mutable {

public:
  Switch(double p_x, double p_y, Object* p_object = nullptr);
  ~Switch() override;

  Type GetType() const override;
  std::string GetName() const override;
  ppxl::Point GetObjectPosition() const override;

};

#endif
