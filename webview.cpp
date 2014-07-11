#include "webview.h"

#ifdef QT_BUILD_WITH_OPENGL
#include <QtOpenGL/QGLWidget>
#include <QCoreApplication>
#endif

#ifdef QT_BUILD_WITH_QML_API
#include <QQmlProperty>
#include <QQuickItem>

#define QML_URL ""

#define QML_DATA        \
"\n"                    \
"import QtQuick 2.0\n"  \
"import QtWebKit 3.0\n" \
"\n"                    \
"WebView {\n"           \
"    id : webView\n"    \
"}"
#endif

WebView* WebView::webview=NULL;

WebView::WebView()
{}

WebView::WebView(const WebView&)
{}

WebView& WebView::operator=(const WebView&)
{}

WebView& WebView::instance(void)
{
    return *webview;
}

WebView::~WebView()
{
    webview=NULL;
}

WebPage& WebView::page(void)
{
    return _page_;
}

WK1WebView::WK1WebView()
{
    g_view.setScene(new QGraphicsScene(&g_view));
    g_view.scene()->setItemIndexMethod(QGraphicsScene::NoIndex);
    g_view.setAttribute(Qt::WA_DeleteOnClose);
    g_view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    g_view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    g_view.setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    g_view.setFrameStyle(QFrame::NoFrame);
#ifdef QT_BUILD_WITH_OPENGL
    g_view.setViewport(new QGLWidget());
#endif
    g_view.showFullScreen();

    g_view.scene()->addItem(&g_webview);

#ifdef QT_BUILD_WITH_OPENGL
    QCoreApplication* app = QCoreApplication::instance();
    Q_ASSERT(NULL != app);
    app->installEventFilter(&g_view);
#endif
}

WK1WebView& WK1WebView::instance(void)
{
    if (webview==NULL)
        webview = new WK1WebView();

    Q_ASSERT(webview!=NULL);

    return dynamic_cast<WK1WebView&>(WebView::instance()); 
}

WK1WebView::~WK1WebView()
{
#ifdef QT_BUILD_WITH_OPENGL
    QCoreApplication* app = QCoreApplication::instance();
    Q_ASSERT(NULL != app);
    app->removeEventFilter(&g_view);
#endif
}

void WK1WebView::destroy(void)
{
    delete dynamic_cast<WK1WebView*>(webview);
}

void WK1WebView::setViewportUpdateMode(WebView::ViewportUpdateMode mode)
{
    switch(mode)
    {
        case FullViewport :
            g_view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
            break;
        default           :;
    }
}

void WK1WebView::resize(const QSize& _size_)
{
    g_view.resize(_size_);
    g_webview.resize(_size_);
}

void WK1WebView::load(const QUrl& _url_)
{
    g_webview.load(_url_);
}

void WK1WebView::show()
{
    g_webview.show();
}

void WK1WebView::hide()
{
    //No implementation
}

void WK1WebView::setFocus()
{
    g_webview.setFocus();
}

#ifdef QT_BUILD_WITH_QML_API
WK2WebView::WK2WebView() : q_component(&q_engine), q_webview(NULL), q_view(&q_engine, NULL)
{
    //Creation from a local file should be instant. Therefore we keep it simple.
    q_component.setData(QByteArray(QML_DATA), QUrl(QML_URL));

    Q_ASSERT(q_component.status() == QQmlComponent::Ready);

    q_webview = q_component.create();
    Q_ASSERT(q_webview!=0);

    q_view.setResizeMode(QQuickView::SizeViewToRootObject);

    QQuickItem* q_item=dynamic_cast<QQuickItem*>(q_webview);
    Q_ASSERT(q_item!=NULL);

    //Add the qml item to the view
    q_view.setContent(QUrl(QML_URL), &q_component, q_item);
}

WK2WebView& WK2WebView::instance(void)
{
    if (webview==NULL)
        webview = new WK2WebView();

    Q_ASSERT(webview!=NULL);

    return dynamic_cast<WK2WebView&>(WebView::instance()); 
}

WK2WebView::~WK2WebView()
{
    delete q_webview;
    q_webview=NULL;
}

void WK2WebView::destroy(void)
{
    delete dynamic_cast<WK2WebView*>(webview);
}

void WK2WebView::setViewportUpdateMode(WebView::ViewportUpdateMode mode)
{
    switch(mode)
    {
        case FullViewport :
            //No implementation
            break;
        default           :;
    }
}

void WK2WebView::resize(const QSize& _size_)
{
    Q_ASSERT(q_webview != NULL);

    QQmlProperty x(q_webview, "x");
    if(x.isValid())
        x.write(0);

    QQmlProperty y(q_webview, "y");
    if(y.isValid())
        y.write(0);

    QQmlProperty width(q_webview, "width");
    if(width.isValid())
        width.write(_size_.width());

    QQmlProperty height(q_webview, "height");
    if(height.isValid())
        height.write(_size_.height());
}

void WK2WebView::load(const QUrl& _url_)
{
    Q_ASSERT(q_webview != NULL);

    QQmlProperty property(q_webview, "url");
    if(property.isValid())
        property.write(_url_);
}

void WK2WebView::show()
{
    Q_ASSERT(q_webview != NULL);

    QQmlProperty property(q_webview, "visible");
    if(property.isValid())
        property.write(true);

    q_view.show();
}

void WK2WebView::hide()
{
    //No implementation

/*
    q_view.hide();

    Q_ASSERT(q_webview != NULL);

    QQmlProperty property(q_webview, "visible");
    if(property.isValid())
        property.write(false);
*/
}

void WK2WebView::setFocus()
{
    Q_ASSERT(q_webview != NULL);

    QQmlProperty property(q_webview, "focus");
    if(property.isValid())
        property.write(false);
}
#endif