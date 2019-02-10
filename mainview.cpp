#include "mainview.h"
#include "ui_mainview.h"

MainView::MainView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
    m_pCodec = avcodec_find_decoder(AV_CODEC_ID_H264);
    m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
    avcodec_open2(m_pCodecCtx, m_pCodec, nullptr);
    m_pFrame = av_frame_alloc();
    socket = new QTcpSocket(this);
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
    unsigned char *dataf = (unsigned char*) data.data();
    AVPacket emptyPacket;
    av_init_packet(&emptyPacket);
    emptyPacket.data = dataf;
    emptyPacket.size = data.size();
    int test = avcodec_send_packet(m_pCodecCtx, &emptyPacket);
    int test2 = avcodec_receive_frame(m_pCodecCtx, m_pFrame);
    //int nres=avcodec_decode_video2(m_pCodecCtx,m_pFrame,&frame_finished, &emptyPacket);

    if (test == 0 && test2 == 0) {
        qDebug() << "okey?";
        Mat mat(m_pCodecCtx->height, m_pCodecCtx->width, CV_8UC3, m_pFrame->data[0], (unsigned long) m_pFrame->linesize[0]);
        auto test = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        ui->image->setPixmap(QPixmap::fromImage(test));

//        cvtColor(mat, mat);
//        imshow("frame", mat);
    } else {
        qDebug() << "not okey?";
    }
    //process.write(data);
}

void MainView::initializeProcess()
{
    process.start("ffplay", QStringList() << "-framerate" << "60" << "-");
}
