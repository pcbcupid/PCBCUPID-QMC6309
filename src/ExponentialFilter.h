#pragma once

class ExponentialFilter {
  float alpha;
  float filteredValue;

public:
  ExponentialFilter(float weight, float initial) {
    alpha = weight;
    filteredValue = initial;
  }

  void update(float newValue) {
    filteredValue = alpha * newValue + (1.0f - alpha) * filteredValue;
  }

  float get() const {
    return filteredValue;
  }

  void setWeight(float weight) {
    alpha = weight;
  }

  void reset(float value) {
    filteredValue = value;
  }
};
