#include "angle_calculation.h"


ENCODER::ENCODER(float *coefs_f1, float *coefs_f2, float *coefs_f3, float *coefs_f4, float *edge_value,
                 float error_bias, fit_model model) {
  coefficients_f1 = coefs_f1;
  coefficients_f2 = coefs_f2;
  coefficients_f3 = coefs_f3;
  coefficients_f4 = coefs_f4;
  upside_edge = edge_value[1];
  downside_edge = edge_value[0];
  output_error_bias = error_bias;
  fitModel = model;
}


float ENCODER::calculate_angle(float chA, float chB, float chC, float chD) {
  float cfine, crough;
  float temp;
  cfine = chB - chC;
  crough = chD - chA - crough_offset;

  if(fitModel == LINEAR){
    if((cfine > downside_edge) && (cfine < upside_edge)){
      if(crough > 0) return linear_func(coefficients_f1, cfine);
      else if(crough < 0) return linear_func(coefficients_f2, cfine);
    }
    else if(cfine < downside_edge){
      temp = linear_func(coefficients_f3, crough);
      return (temp<0?temp+360:temp);
    }
    else if(cfine > upside_edge) return linear_func(coefficients_f4, crough);
  }

  else if(fitModel == POLY2){
    if((cfine > downside_edge) && (cfine < upside_edge)){
      if(crough > 0) return poly2_func(coefficients_f1, cfine);
      else if(crough < 0) return poly2_func(coefficients_f2, cfine);
    }
    else if(cfine <= downside_edge){
      temp = poly2_func(coefficients_f3, crough);
      return (temp<0?temp+360:temp);
    }
    else if(cfine >= upside_edge) return poly2_func(coefficients_f4, crough);
  }
  return 0;
}

float ENCODER::linear_func(float *coefs, float x) const {
  return coefs[0]*x + coefs[1] + output_error_bias;
}

float ENCODER::poly2_func(float *coefs, float x) const {
  return coefs[0]*x*x + coefs[1]*x + coefs[2] + output_error_bias;
}




