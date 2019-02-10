#include "mainview.h"
#include "ui_mainview.h"

#include <QThread>

MainView::MainView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    m_pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
    avcodec_open2(m_pCodecCtx, m_pCodec, nullptr);
    connect(socket, SIGNAL(readyRead()), SLOT(handlePictureByteArray()));
    //initializeProcess();
    socket->connectToHost("192.168.31.221", 1337);

}

MainView::~MainView()
{
    delete ui;
}

void MainView::handlePictureByteArray()
{
    QByteArray data = socket->readAll();
    m_pFrame = av_frame_alloc();
    unsigned char *dataf = (unsigned char*) data.data();
    AVPacket emptyPacket;
    av_init_packet(&emptyPacket);
    emptyPacket.data = dataf;
//    m_pCodecCtx->width = 720;
//    m_pCodecCtx->height = 1280;
    emptyPacket.size = data.size();
    int test = avcodec_send_packet(m_pCodecCtx, &emptyPacket);
    int test2 = avcodec_receive_frame(m_pCodecCtx, m_pFrame);

    if (test == 0 && test2 == 0) {
        qDebug() << "okey?";
        Mat mat(m_pCodecCtx->height, m_pCodecCtx->width, CV_8UC1,  m_pFrame->data[0]);
        Mat exmat(m_pCodecCtx->height, m_pCodecCtx->width, CV_8UC1,  m_pFrame->extended_data[0]);
        imshow("mat", mat);
        imshow("exmat", exmat);

        //Mat changed;
//        cvtColor(mat, mat, COLOR_BGR2RGB);
        //auto test = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        //ui->image->setPixmap(QPixmap::fromImage(test));

        //imshow("changed", changed);
    } else {
        qDebug() << "not okey?";
    }
    //process.write(data);
}

void MainView::initializeProcess()
{
    process.start("ffplay", QStringList() << "-framerate" << "60" << "-");
}
