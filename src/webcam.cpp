#include <webcam.hpp>
#include <unistd.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/tracking.hpp>

#define WEBCAM_DEFAULT_FRAME_DELAY 30

#define WEBCAM_FLIP

Webcam::Webcam()
{
    thread_run = false;
    frame_delay = WEBCAM_DEFAULT_FRAME_DELAY;

    /* find webcam in devices list */
    webcam = new cv::VideoCapture(CV_CAP_ANY);
    is_work = webcam->isOpened();
}

Webcam::~Webcam()
{
    end();
    delete webcam;
}

int Webcam::get_width(void)
{
    if (!is_work_correctly())
        return -1;

    return (int)webcam->get(CV_CAP_PROP_FRAME_WIDTH);
}

int Webcam::get_height(void)
{
    if (!is_work_correctly())
        return -1;

    return (int)webcam->get(CV_CAP_PROP_FRAME_HEIGHT);
}

bool Webcam::is_work_correctly(void)
{
    return is_work;
}

void Webcam::start()
{
    if (!is_work_correctly())
        return;

    if (!thread_run)
    {
        thread_run = true;
        thread = std::thread(&Webcam::run, this);
    }
}

void Webcam::end()
{
    if (!is_work_correctly())
        return;

    if (thread_run)
    {
        thread_run = false;
        if(thread.joinable())
            thread.join();
    }
}

void Webcam::run()
{
    /* tracker needed data */
    cv::Ptr<cv::Tracker> tracker;
    const cv::Scalar red_scalar(0, 0, 255);
    bool first_time = true;
    cv::Rect2d bbox;
    const std::string roi_window_name = std::string("ROI");

    /* create window*/
    window_create();

    if (!is_work_correctly())
        return;

    while (thread_run)
    {
        /* capture single frame */
        capture_frame(frame);

        /* create image from frame*/
        image_create(frame);

        if (first_time)
        {
            /* Create KCF tracker */
            tracker = cv::TrackerKCF::create();

            const bool from_center = false;
            first_time = false;

            /* select ROI in gui */
            bbox = cv::selectROI(roi_window_name, image, from_center);

            /* destroy window after selecting */
            cv::destroyWindow(roi_window_name);
            cv::rectangle(image, bbox, red_scalar);

            /* start tracking object in ROI */
            tracker->init(image, bbox);
        }
        else
        {
            /* just update tracker */
            if (tracker->update(image, bbox))
                cv::rectangle(image, bbox, red_scalar);
            else
            {
                const std::string text = std::string("Object lost !!!");
                const cv::Point2f point(10, 20);
                const cv::Scalar scalar(0, 0, 255);
                const int font_face = cv::FONT_HERSHEY_PLAIN;
                const double font_scale = 1.0;
    
                cv::putText(image, text, point, font_face, font_scale, scalar);
            }
        }

        /* show result */
        image_show();

        /* wait "frame_delay" needed for capture frames from webcam */
        cv::waitKey(frame_delay);
    }
}

int Webcam::capture_frame(cv::Mat &frame)
{
    if (!is_work_correctly())
        return 1;

    return (int)webcam->read(frame);
}

int Webcam::image_create(const cv::Mat &frame)
{
    image = frame.clone();
#ifdef WEBCAM_FLIP
    cv::flip(image, image, 0);
#endif
    return 0;
}

void Webcam::window_create(void)
{
    cv::namedWindow(real_window_name);
}

void Webcam::image_show(void)
{
    cv::imshow(real_window_name, image);
}