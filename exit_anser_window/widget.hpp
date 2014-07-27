#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <QWidget>

/**QT4 도 비슷한 방법입니다..\n
frmMain.ui 에서 TfrmMain의 디자인 정보를 가져와서 그 디자인에 따라 frmMain을 구성하겠다는 것입니다.\n
 frmMain.h 내용을 다시 보면 UI::TfrmMain ui; 를 보실 수 있습니다.
 */
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    virtual void keyReleaseEvent(QKeyEvent *event);
    
private:
    Ui::Widget *ui; ///< formSystemOut의 UI를 사용할 수 있도록 ui를 선언

public slots:
    void shutdown();
    void logout();
    void switchUsers();
    void reboot();

};

#endif // WIDGET_HPP
