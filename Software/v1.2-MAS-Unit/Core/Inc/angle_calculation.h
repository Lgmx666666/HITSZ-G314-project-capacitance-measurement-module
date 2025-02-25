#ifndef MAS_UNIT_ANGLE_CALCULATION_H
#define MAS_UNIT_ANGLE_CALCULATION_H

#include "main.h"

enum fit_model{
    LINEAR = 0,
    POLY2,
};

class ENCODER{
public:
  ENCODER(float* coefs_f1,
          float* coefs_f2,
          float* coefs_f3,
          float* coefs_f4,
          float* edge_value,
          float error_bias,
          fit_model model);

  float linear_func(float* coefs, float x) const;
  float poly2_func(float* coefs, float x) const;
  float calculate_angle(float chA, float chB, float chC, float chD);

private:
    constexpr static const float crough_offset = 0.35; // 0.245
    float* coefficients_f1{};
    float* coefficients_f2{};
    float* coefficients_f3{};
    float* coefficients_f4{};
    float upside_edge = 0;
    float downside_edge = 0;
    float output_error_bias = 0;

    fit_model fitModel = LINEAR;

};

#endif //MAS_UNIT_ANGLE_CALCULATION_H
