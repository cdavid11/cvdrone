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
};

/*
*/
void line_main();

/*
*/
void detect_lines(cv::Mat &original_frame, double scale_factor);

#endif
