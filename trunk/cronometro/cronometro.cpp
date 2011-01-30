#include <QApplication>
#include <QFontMetrics>
#include <QMessageBox>
#include <QMoveEvent>
#include <QPushButton>
#include <QResizeEvent>
#include <QSettings>
#include <QTime>
#include <QTimer>
#include <QWidget>

const int borda = 10;

class cronometro : public QWidget {
    Q_OBJECT;
    private:
        QPushButton *btnTime;
        QPushButton *btnReset;
        QSettings *conf;
        QTimer *timTimer;
        QTimer *timTimerConf;
        int h, m, s;
        void resizeEvent (QResizeEvent *);
        void moveEvent (QMoveEvent *);
        void escreveTXT ();
    public:
        cronometro ();
    private slots:
        void btnReset_clicked (bool);
        void btnTime_clicked (bool);
        void timTimer_timeout ();
        void timTimerConf_timeout ();
};

cronometro::cronometro () {
    this->conf = new QSettings ("cronometro", QString (), this);
    this->conf->sync ();
    this->h = this->conf->value ("tempo/hora", 0).toInt ();
    this->m = this->conf->value ("tempo/minuto", 0).toInt ();
    this->s = this->conf->value ("tempo/segundo", 0).toInt ();
    this->timTimer = new QTimer (this);
    this->timTimer->setInterval (1000);
    this->timTimer->setSingleShot (false);
    QObject::connect (this->timTimer, SIGNAL (timeout ()), this, SLOT (timTimer_timeout ()));
    this->btnTime = new QPushButton (this);
    this->btnTime->move (10, 10);
    this->btnTime->setToolTip ("Clique para iniciar/parar o cronometro.");
    this->btnTime->setFlat (true);
    this->btnTime->setFocusPolicy (Qt::NoFocus);
    this->btnTime->setCursor (Qt::PointingHandCursor);
    this->btnTime->show ();
    QObject::connect (this->btnTime, SIGNAL (clicked (bool)), this, SLOT (btnTime_clicked (bool)));
    this->btnReset = new QPushButton (this);
    this->btnReset->setText ("Reiniciar");
    this->btnReset->adjustSize ();
    this->btnReset->show ();
    QObject::connect (this->btnReset, SIGNAL (clicked (bool)), this, SLOT (btnReset_clicked (bool)));
    this->timTimerConf = new QTimer (this);
    this->timTimerConf->setInterval (5000);
    this->timTimerConf->setSingleShot (false);
    QObject::connect (this->timTimerConf, SIGNAL (timeout ()), this, SLOT (timTimerConf_timeout ()));
    this->timTimerConf->start ();
    this->resize (this->conf->value ("janela/largura", 470).toInt(),
                  this->conf->value ("janela/altura", 140).toInt());
    this->move (this->conf->value ("janela/posicao_x", 20).toInt(),
                this->conf->value ("janela/posicao_y", 20).toInt());
    this->show ();
    this->setWindowTitle ("AMG Cronometer 1.0 [Parado]");
    this->escreveTXT ();
    QResizeEvent ev (this->size (), this->size ());
    this->resizeEvent (&ev);
}

void cronometro::escreveTXT () {
    this->btnTime->setText (QString ("%1h %2min %3s")
                                    .arg (this->h, 2, 10, QChar('0'))
                                    .arg (this->m, 2, 10, QChar('0'))
                                    .arg (this->s, 2, 10, QChar('0')));
    this->conf->setValue ("tempo/hora", this->h);
    this->conf->setValue ("tempo/minuto", this->m);
    this->conf->setValue ("tempo/segundo", this->s);
}

void cronometro::timTimerConf_timeout () {
    this->conf->sync ();
}

void cronometro::timTimer_timeout () {
    this->s++;
    if (this->s > 59) {
        this->s = 0;
        this->m++;
        if (this->m > 59) {
            this->m = 0;
            this->h++;
            if (this->h > 23) {
                this->h = 0;
            }
        }
    }
    this->escreveTXT ();
}

void cronometro::btnReset_clicked (bool) {
    if (QMessageBox::question (this, "Reiniciar o cronometro", "Tem certeza que voce deseja reiniciar o cronometro?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
        this->timTimer->stop ();
        this->h = 0;
        this->m = 0;
        this->s = 0;
        this->escreveTXT ();
        this->setWindowTitle ("AMG Cronometer 1.0 [Parado]");
    }
}

void cronometro::btnTime_clicked (bool) {
    if (this->timTimer->isActive ()) {
        this->timTimer->stop ();
        this->setWindowTitle ("AMG Cronometer 1.0 [Parado]");
    } else {
        this->timTimer->start ();
        this->setWindowTitle ("AMG Cronometer 1.0 [Rodando]");
    }
}

void cronometro::resizeEvent (QResizeEvent *ev) {
    QFont fon (this->btnTime->font ());
    QString txt ("00h 00min 00s");
    QRect rct (this->btnTime->rect());
    int nfon = fon.pointSize ();
    int calc1, calc2;
    calc1 = (ev->size().width() - this->btnReset->width()) / 2;
    calc2 = (ev->size().height() - borda - this->btnReset->height());
    this->btnReset->move (calc1, calc2);
    calc2 -= borda + borda;
    calc1 = ev->size().width() - borda - borda;
    this->btnTime->resize (calc1, calc2);
    while (nfon < 1000) {
        QFontMetrics medi (fon);
        QRect cabe (medi.boundingRect (txt));
        if (rct.width () > cabe.width () && rct.height () > cabe.height ()) {
            fon.setPointSize (++nfon);
        } else {
            break;
        }
    }
    while (nfon > 0) {
        QFontMetrics medi (fon);
        QRect cabe (medi.boundingRect (txt));
        if (rct.width () > cabe.width () && rct.height () > cabe.height ()) {
            break;
        } else {
            fon.setPointSize (--nfon);
        }
    }
    this->btnTime->setFont (fon);
    this->conf->setValue ("janela/largura", ev->size().width());
    this->conf->setValue ("janela/altura", ev->size().height());
}

void cronometro::moveEvent (QMoveEvent *ev) {
    this->conf->setValue ("janela/posicao_x", ev->pos().x());
    this->conf->setValue ("janela/posicao_y", ev->pos().y());
}

int main (int argc, char **argv) {
    QApplication prog (argc, argv);
    cronometro cron;
    return (prog.exec ());
}

#include "cronometro.moc"
