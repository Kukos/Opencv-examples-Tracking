#ifndef __WEBCAM__HPP__
#define __WEBCAM__HPP__

#include <thread>
#include <atomic>
#include <opencv2/opencv.hpp>


/*
    Class implements webcam functions, getting frames, tracking object
    all works in background in thread

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0+
*/

class Webcam
{
public:

    /*
        CONSTRUCTOR:
        Only set variables, doesn't run thread
     */
    Webcam(void);

    /*
        DESCTRUCTOR:
        End thread work
    */
    virtual ~Webcam(void);

    /*
        Check webcam correctness

        PARAMS
        NO PARAMS

        RETURN:
        true iff webcam works
        false iff webcam doesn't work
    */
    bool is_work_correctly(void);

    /*
        start work in background ( thread )

        PARAMS:
        NO PARAMS

        RETURN:
        This is a void function
    */
    void start(void);

    /*
        end work in background ( thread )

        PARAMS:
        NO PARAMS

        RETURN:
        This is a void function
    */
    void end(void);

    /*
        Get webcam frame width

        PARAMS:
        NO PARAMS

        RETURN:
        -1 if failure
        width if success
    */
    int get_width(void);

    /*
        Get webcam frame height

        PARAMS:
        NO PARAMS

        RETURN:
        -1 if failure
        height if success
    */
    int get_height(void);

private:

    cv::VideoCapture *webcam; /* webcam */
    cv::Mat image; /* current image */
    cv::Mat frame; /* current captured frame */
    const std::string real_window_name = std::string("REAL");

    std::thread thread; /* our thread */

    /* Control bool variable need to be atomic */
    std::atomic<bool> thread_run; /* true --> thread work, false --> thread end work */
    
    /* returned value by constructor during reading devices */
    bool is_work;

    /* delay between frame capture in waitkey */
    int frame_delay;

    /*
        Thread main function

        PARAMS:
        NO PARAMS


        RETURN:
        This is a void function
    */
    void run(void);

    /*
        Capture frame

        PARAMS:
        @OUT frame - Reference to Mat image

        RETURN:
        0 if success
        Non-zero if failure
    */
    int capture_frame(cv::Mat &frame);

    /*
       Create image from frame

       PARAMS:
       @IN frame - ref to frame

       RETURN:
       0 if success
       Non-zero if failure
    */
    int image_create(const cv::Mat &frame);

    /*
        Create window

        PARAMS
        NO PARAMS

        RETURN
        This is a void function
    */
    void window_create(void);

    /*
        Show image

        PARAMS
        NO PARAMS

        RETURN
        This is a void function
    */
    void image_show(void);
};

#endif
