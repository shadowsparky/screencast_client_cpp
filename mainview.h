#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QMainWindow>
#include <QProcess>
#include <QTcpSocket>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
extern "C" {
    #include <libavcodec/avcodec.h>
}
using namespace cv;

namespace Ui {
class MainView;
}

class MainView : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainView(QWidget *parent = nullptr);
    void initializeProcess();
    void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt);
    ~MainView();

public slots:
    void handlePictureByteArray();

private:
    Ui::MainView *ui;
    QTcpSocket *socket;
    AVCodec *codec;
    AVCodecParserContext *parser;
    AVCodecContext *c = NULL;
    AVFrame *picture;
    AVPacket *pkt;
//    QProcess process;
//    AVPacket *packet;
//    AVCodec *m_pCodec;
//    AVCodecContext *m_pCodecCtx;
//    AVFrame *m_pFrame;
};

#endif // MAINVIEW_H
