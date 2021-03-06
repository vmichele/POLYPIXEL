#ifndef DISPOSABLE_HXX
#define DISPOSABLE_HXX

#include "Core/Objects/Mutables/Mutable.hxx"

class Disposable: public Mutable {

public:
  Disposable(double p_x, double p_y, Object* p_object = nullptr);
  ~Disposable() override;

  void UseObject();

  CategoryType GetCategoryType() const override;
  ObjectType GetObjectType() const override;
  std::string GetName() const override;
  ppxl::Point GetObjectPosition() const override;

private:
  bool m_objectUsed;
};

#endif
