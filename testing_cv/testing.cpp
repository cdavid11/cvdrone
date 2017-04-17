#include <opencv2/opencv.hpp>
#include "../src/lineFollowing/lineFollowing.h"


using namespace cv;
using namespace std;

<<<<<<< HEAD
int main(int argc, char **argv) {
  string filename;
  if (argc < 2) {
    filename = "test1.avi";
  } else {
    filename = argv[1];
  }
=======
int main() {
  string filename = "test1.avi";
>>>>>>> 71dc6bfdf4ba6488abdee2302d77d2b03788423e
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