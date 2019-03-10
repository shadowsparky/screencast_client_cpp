#include "mainview.h"
#include "ui_mainview.h"

#include <QThread>

MainView::MainView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    avcodec_register_all();
    pkt = av_packet_alloc();
    if (!pkt) {
        qDebug() << "Error in packet initialization";
        exit(-1);
    }
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        qDebug() << "codec not found";
        exit(-2);
    }
    parser = av_parser_init(codec->id);
    if (!parser) {
        qDebug() << "parser not found";
        exit(-3);
    }
    c = avcodec_alloc_context3(codec);
    picture = av_frame_alloc();
    if (avcodec_open2(c, codec, NULL) < 0) {
        qDebug() << "could not open codec";
        exit(-4);
    }
//    initializeProcess();
    connect(socket, SIGNAL(readyRead()), SLOT(handlePictureByteArray()));
    socket->connectToHost("192.168.31.221", 1337);

}

MainView::~MainView()
{
    delete ui;
}

void MainView::decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt)
{
    char buf[1024];
    int ret;
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
    }
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }

        Mat mat1(c->height, c->width, CV_8UC1, picture->data[0], (unsigned long) picture->linesize[0]);
        imshow("frame", mat1);

//        printf("saving frame %3d\n", dec_ctx->frame_number);
//        fflush(stdout);
        /* the picture is allocated by the decoder. no need to
           free it */
//        snprintf(buf, sizeof(buf), filename, dec_ctx->frame_number);
    }
}



void MainView::handlePictureByteArray()
{
    int ret = 0;
    QByteArray data = socket->readAll();
    uint8_t *dataf = (uint8_t*) data.data();
    size_t data_size = data.size();
    if (data.size() > 0) {
        ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size, dataf, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (ret < 0) {
            qDebug() << "Error while parsing";
            return;
        }
        data += ret;
        data_size -= ret;
        if (pkt->size) {
            decode(c, picture, pkt);
        }
    }
}

void MainView::initializeProcess()
{
//    process.start("ffplay", QStringList() << "-framerate" << "60" << "-");
}
