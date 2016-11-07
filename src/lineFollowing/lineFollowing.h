/*

*/
#ifndef LINE_FOLLOWING_HEADER
#define LINE_FOLLOWING_HEADER

#include "../ardrone/ardrone.h"
#include "../structures.h"

class Control;

/*
*/
class LineFollowing {
public:
  LineFollowing(Control *control);

  void close();

  void fly();

private:
  Control *control_ptr;

  void detect_lines(cv::Mat &original_frame);

  double minH, minS, minV, maxH, maxS, maxV;
  double width, height;
  std::vector<cv::Vec2f> found_lines;

  double distance_from_center(float rho, float theta, double width, double height);

  cv::Vec2i find_intersection(cv::Vec2f a, cv::Vec2f b);
};


#endif
