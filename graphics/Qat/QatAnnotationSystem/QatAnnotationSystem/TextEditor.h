#ifndef _TextEditor_h_
#define _TextEditor_h_
#include "ui_TextEditor.h"
#include "QatPlotWidgets/characterwidget.h"
#include <QScrollArea>

class TextEditor: public QWidget
 {
     Q_OBJECT

 public:

     // Constructor:
     TextEditor(QWidget *parent = 0);

     // Get the text editor:
     const QTextEdit *textEdit() const;
     QTextEdit *textEdit(); 
     void insertDate();

 private slots:

     void textFamily( const QString &f );
     void textSize(int i);
     void textBold();
     void textItalic();
     void textScript();
     void textHal();  // horizontal alignment.
     void textColor();
     void toggleChars();
     void updateCharFormat(const QTextCharFormat &f);
     void updateAlignment();

     void selectionChanged(bool copyAvailableFlag);
     void imageResize(int percent);

 signals:

     void imageSelected(const QString & imageFile);

 private:

     Ui::TextEditor ui;
     QScrollArea     *frame;
     CharacterWidget *cw;
     const QMimeData *selectedMimeData;
     std::string     selectedFileName;
     int             selHeight;
     int             selWidth;
 };
#endif

