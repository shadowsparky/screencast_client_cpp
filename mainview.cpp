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
    RGBPicture = av_frame_alloc();
    int width = 1080;
    int height = 1920;
    bytes = avpicture_get_size(AV_PIX_FMT_RGB24, width, height);
    buffer=(uint8_t *)av_malloc(bytes*sizeof(uint8_t));
    avpicture_fill((AVPicture *)RGBPicture, buffer, AV_PIX_FMT_RGB24,
                    width, height);
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

int MainView::decode_write_frame()
{
    int len, got_frame;
//    char buf[1024];
    struct SwsContext *convert_ctx;

    //Use avcodec_send_packet() and avcodec_receive_frame()
    len = avcodec_send_packet(c, pkt);
    if (len != 0) {
        return 0;
    }
    len = avcodec_receive_frame(c, picture);
    if (len != 0) {
        return 0;
    }
    qDebug() << "3";
    if (got_frame >= 0) {
//        fflush(stdout);
        int w = c->width;
        int h = c->height;
        qDebug() << "3.5";
        convert_ctx = sws_getContext(w, h, c->pix_fmt,
                            w, h, AV_PIX_FMT_BGR24, SWS_BICUBIC,
                            NULL, NULL, NULL);
        qDebug() << "4";
        if(convert_ctx == NULL) {
//            fprintf(stderr, "Cannot initialize the conversion context!\n");
            exit(1);
        }

        sws_scale(convert_ctx, picture->data,
                    picture->linesize, 0,
                    h,
                    RGBPicture->data, RGBPicture->linesize);

        qDebug() << "5";
        Mat mat(c->height, c->width, CV_8UC3, RGBPicture->data[0], RGBPicture->linesize[0]);
        imshow("frame", mat);
//            (*frame_count)++;
    }
    if (pkt->data) {
        pkt->size -= len;
        pkt->data += len;
    }
    return 0;
}


void MainView::handlePictureByteArray()
{
    int ret = 0;
//    qDebug() << "0";
    auto test = socket->readAll();
    qDebug() << socket->size();
//    pkt->data = (uint8_t*) test.data();
//    pkt->size= test.size();
//    size_t data_size = 1000;
//    unsigned char dataf[data_size];
//    socket->read((char*)dataf, data_size);
//    pkt->data = dataf;
//    pkt->size = data_size;
//    if (data.size() > 0) {
//        ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size, dataf, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
//        if (ret < 0) {
//            qDebug() << "Error while parsing";
//            return;
//        }
//        qDebug() << "1";
//        data += ret;
//        data_size -= ret;
//        if (pkt->size) {
//     decode_write_frame();
//        }
//    qDebug() << "2";
//    }
}

void MainView::initializeProcess()
{
//    process.start("ffplay", QStringList() << "-framerate" << "60" << "-");
}
