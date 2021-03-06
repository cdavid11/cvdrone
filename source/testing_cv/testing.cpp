#include <opencv2/opencv.hpp>
#include "../drone/lineFollowing/lineFollowing.h"


using namespace cv;
using namespace std;


int main(int argc, char **argv) {
  string filename;
  if (argc < 2) {
    fprintf(stderr, "Using default filename\n");
    filename = "test1.avi";
  } else {
    filename = argv[1];
  }
  fprintf(stderr, "Opened %s\n", filename.c_str());
  VideoCapture capture(filename);
  Mat frame;
  if (!capture.isOpened()) {
    fprintf(stderr, "Error when reading file\n");
  }
  namedWindow("output", 1);
  while (true) {
//    capture >> frame;
    if (!capture.read(frame))
      break;

    LineFollowing l = LineFollowing();
    l.detect_lines(frame);
    imshow("w", frame);
    waitKey(20); // waits to display frame
  }
  waitKey(0);

  return 1;
}