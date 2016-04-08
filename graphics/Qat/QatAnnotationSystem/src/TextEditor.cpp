#include "QatAnnotationSystem/TextEditor.h"
#include <QColorDialog>
#include <QClipboard>
#include <QPixmap>
#include <QMimeData>
#include <QImage>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <time.h>

inline std::string getTag(const std::string & html, const std::string & tag) {
  size_t begin= html.find(tag);
  if (begin==html.npos) { 
    return "";
  }
  else {
    begin=begin+tag.size()+1;
  }
  size_t end = html.find("\"",begin);
  if (end==html.npos) { 
    return "";
  }
  return html.substr(begin,end-begin);
}
// For returning height, width. 
inline int getValue (const std::string & html, const std::string & tag) {
  const std::string valString=getTag(html,tag);
  std::istringstream valStream(valString);
  int retVal;
  if (valStream >> retVal) return retVal;
  return 0;
  
}

inline void setValue(std::string & html, const std::string & tag, int  value) {
  size_t begin= html.find(tag);
  if (begin==html.npos) { 
    return;
  }
  else {
    begin=begin+tag.size()+1;
  }
  size_t end = html.find("\"",begin);
  if (end==html.npos) { 
    return;
  }

  std::ostringstream valStream;
  valStream << value;

  html.replace(begin,end-begin, valStream.str());

}


TextEditor::TextEditor(QWidget *parent): QWidget(parent),selectedMimeData(NULL),selHeight(0),selWidth(0) {
  ui.setupUi(this);
  connect (ui.boldButton,         SIGNAL(clicked()), this, SLOT(textBold()));
  connect (ui.italicButton,       SIGNAL(clicked()), this, SLOT(textItalic()));
  connect (ui.scriptButton,       SIGNAL(clicked()), this, SLOT(textScript()));
  connect (ui.halButton,          SIGNAL(clicked()), this, SLOT(textHal()));
  connect (ui.fontComboBox,       SIGNAL(activated( const QString & ) ), this, SLOT(textFamily(const QString &)));
  connect (ui.fontSizeSpinBox,    SIGNAL(valueChanged(int)),             this, SLOT(textSize(int)));
  connect (ui.colorButton,        SIGNAL(clicked()),                     this, SLOT(textColor()));        
  connect (ui.specialCButton,     SIGNAL(clicked()),                     this, SLOT(toggleChars()));
  connect (ui.imageSizeSpinBox,   SIGNAL(valueChanged(int)),             this, SLOT(imageResize(int)));
  QColor color("black");

  QPalette palette = ui.colorButton->palette();
  palette.setColor(QPalette::Button,color);
  ui.colorButton->setAutoFillBackground(true);
  ui.colorButton->setPalette(palette);
  
  
  frame = new QScrollArea(0);
  frame->setObjectName(QString::fromUtf8("frame"));
  QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(13), static_cast<QSizePolicy::Policy>(13));
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
  frame->setSizePolicy(sizePolicy);
  frame->setMinimumSize(QSize(0, 400));


  cw=new CharacterWidget(0);
  frame->setWidget(cw);
  connect (ui.fontComboBox,       SIGNAL(activated( const QString &)),   cw,   SLOT(updateFamily(const QString & )));
  connect (cw, SIGNAL(characterSelected(const QString &)), ui.textEdit, SLOT(insertPlainText(const QString & )));
  
  cw->updateSize("12");

  connect(ui.textEdit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)), this, SLOT(updateCharFormat(const QTextCharFormat &)));
  connect(ui.textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(updateAlignment()));
  connect(ui.textEdit, SIGNAL(copyAvailable(bool)), this, SLOT(selectionChanged(bool)));
  frame->hide();
  setMinimumSize(600,200);
  resize(minimumSize());

  QTextCharFormat format;
  QFont font;
  font.setFamily("Sans Serif");
  font.setPointSize(12);
  format.setFont(font);
  ui.textEdit->setCurrentCharFormat(format);
  ui.textEdit->setFocus();
  ui.textEdit->setText(QString(""));

  ui.imageSizeSpinBox->hide();
  ui.imageSizeLabel->hide();

}

const QTextEdit *TextEditor::textEdit() const {
  return ui.textEdit;
}


QTextEdit *TextEditor::textEdit() {
  return ui.textEdit;
}




void TextEditor::textFamily( const QString &f )
{
  QTextEdit * tE = ui.textEdit;
  QTextCharFormat format; 
  format.setFontFamily(f);
  tE->mergeCurrentCharFormat(format);
}


void TextEditor::textSize( int i )
{
  QTextEdit * tE = ui.textEdit;
  QTextCharFormat format; 
  format.setFontPointSize(i);
  tE->mergeCurrentCharFormat(format);
}


void TextEditor::textBold()
{
 
  QFont font=ui.boldButton->font();
  bool flag=font.bold();
  flag = !flag;
  font.setBold(flag);
  ui.boldButton->setFont(font);


  QTextEdit * tE = ui.textEdit;
  QTextCharFormat format; 
  format.setFontWeight(flag ?  QFont::Bold : QFont::Normal );
  tE->mergeCurrentCharFormat(format);

}

void TextEditor::textItalic()
{
  QFont font=ui.italicButton->font();
  bool flag=font.italic();
  flag = !flag;
  font.setItalic(flag);
  ui.italicButton->setFont(font);


  QTextEdit * tE = ui.textEdit;
  QTextCharFormat format; 
  format.setFontItalic(flag);
  tE->mergeCurrentCharFormat(format);
}


void TextEditor::textScript()
{
  QTextEdit * tE = ui.textEdit;
  QTextCharFormat format;

  QString  text = ui.scriptButton->text();
  if (text==QString("-")) {
    ui.scriptButton->setText(QString("^"));
    format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
  }
  if (text==QString("^")){
    ui.scriptButton->setText(QString("_"));  
    format.setVerticalAlignment(QTextCharFormat::AlignSubScript);
  } 
  if (text==QString("_")) {
    ui.scriptButton->setText(QString("-"));
    format.setVerticalAlignment(QTextCharFormat::AlignNormal);
  }
  
  tE->mergeCurrentCharFormat(format);
}

void TextEditor::textHal()
{
  QTextEdit * tE = ui.textEdit;

  QString  text = ui.halButton->text();
  if (text==QString("<")) {
    ui.halButton->setText(QString("|"));
    tE->setAlignment(Qt::AlignHCenter);
  }
  if (text==QString("|")){
    ui.halButton->setText(QString(">"));  
    tE->setAlignment(Qt::AlignRight);
  } 
  if (text==QString(">")) {
    ui.halButton->setText(QString("<"));
    tE->setAlignment(Qt::AlignLeft);
  }
  
}


void TextEditor::textColor()
{
  QColor textColorVal;
  QColor col = QColorDialog::getColor(textColorVal, this );
  if ( !col.isValid() ) return;

  QPalette palette = ui.colorButton->palette();
  palette.setColor(QPalette::Button,col);
  ui.colorButton->setAutoFillBackground(true);
  ui.colorButton->setPalette(palette);
  QTextEdit * tE = ui.textEdit;
  tE->setTextColor(col);
}

void TextEditor::updateAlignment() {
  QTextEdit * tE = ui.textEdit;
  if (tE->alignment()==Qt::AlignLeft)     ui.halButton->setText(QString("<"));
  if (tE->alignment()==Qt::AlignHCenter)  ui.halButton->setText(QString("|"));
  if (tE->alignment()==Qt::AlignRight)  ui.halButton->setText(QString(">"));
}

void TextEditor::updateCharFormat(const QTextCharFormat &f) {
  static QTextCharFormat df;
  df.setVerticalAlignment(QTextCharFormat::AlignNormal);
  df.setFontItalic(false);
  df.setFontWeight(QFont::Normal);
  df.setFontPointSize(12);
  df.setFontFamily("Sans Serif");

  QTextCharFormat F = f.fontPointSize()!=0.0 ? f: df;
  if (F.verticalAlignment()==QTextCharFormat::AlignNormal)     ui.scriptButton->setText(QString("-"));
  if (F.verticalAlignment()==QTextCharFormat::AlignSubScript)  ui.scriptButton->setText(QString("_"));
  if (F.verticalAlignment()==QTextCharFormat::AlignSuperScript)  ui.scriptButton->setText(QString("^"));
  {
    QFont font=ui.italicButton->font(); 
    font.setItalic (F.fontItalic()); 
    ui.italicButton->setFont(font); 
  }
  {
    QFont font=ui.boldButton->font(); 
    font.setBold(F.fontWeight()==QFont::Bold);
    ui.boldButton->setFont(font); 
  }

  ui.fontSizeSpinBox->setValue(int(F.fontPointSize()+0.5));

  QString fam=F.fontFamily();
  ui.fontComboBox->setEditText(fam);

  
  QColor color=F.foreground().color();
  QPalette palette = ui.colorButton->palette();
  palette.setColor(QPalette::Button,color);
  ui.colorButton->setAutoFillBackground(true);
  ui.colorButton->setPalette(palette);
}



void TextEditor::toggleChars() {
  if (frame->isHidden()) {
    frame->show();
    setMinimumSize(600,600);
    resize(minimumSize());
  }
  else {
    
    frame->hide();
    setMinimumSize(600,200);
    resize(minimumSize());
  }

}

void TextEditor::insertDate() {
  char buffer[1024];
  time_t now = time(NULL);
  asctime_r(localtime(&now),buffer);
  QTextEdit * tE = ui.textEdit;
  tE->insertPlainText(buffer);
}

void TextEditor::selectionChanged(bool flag) {
  selectedFileName="";
  selectedMimeData=NULL;
  // See if the box is shown:
  bool hidden=ui.imageSizeSpinBox->isHidden();
  if (flag) {
    QTextEdit * tE = ui.textEdit;
    tE->copy();
    QClipboard *clipboard=QApplication::clipboard();
    const QMimeData *mimeData=clipboard->mimeData();
    if (mimeData) {
      if (mimeData->hasHtml()) {

	// First extract the name of the file.
	std::string html=mimeData->html().toStdString();
	std::string filename=getTag(html,"src=");
	if (filename!="") {

	  QString fnString=QString::fromStdString(filename);
	  emit imageSelected(fnString);

	  // Read in that file and get the true size.
	  QPixmap px(filename.c_str());
	  
	  selHeight = px.height();
	  selWidth  = px.width();
	  
	  // Get the actual size.  Then, compute the scale factor.
	  int tagHeight=getValue(html,"height=");
	  int tagWidth =getValue(html,"width=");
	  
	  
	  // Set the scale factor for the spin box. 
	  double ratioH = tagHeight/double(selHeight);
	  double ratioW = tagWidth/double(selWidth);
	  int    ratio  = int( 50.0*(ratioH+ratioW));
	  
	  if (hidden) { 

	    ui.imageSizeSpinBox->setValue(ratio);
	  
	    // Record the mime data corresponding to this (it will need to be
	    // replaced when the spin box changes. 
	    
	    selectedFileName=filename;
	    selectedMimeData=mimeData;
	    
	    ui.imageSizeSpinBox->show();
	    ui.imageSizeLabel->show();
	    return;
	  }
	}
      } 
    }      
  }
  ui.imageSizeSpinBox->hide();
  ui.imageSizeLabel->hide();
}
void TextEditor::imageResize(int percent) {
  if (!selectedMimeData) return;

  int newImageHeight=(selHeight*percent)/100;
  int newImageWidth =(selWidth*percent)/100;
 
  std::string html = selectedMimeData->html().toStdString();


  // edit html
  setValue(html,"height=", newImageHeight);
  setValue(html,"width=",  newImageWidth);



  // replace sel html
  QObject::blockSignals(true);
  ui.imageSizeSpinBox->blockSignals(true);
  ui.textEdit->blockSignals(true);
  QTextEdit * tE = ui.textEdit;
  tE->insertHtml(html.c_str()); // Should overwrite existing!
  QTextCursor tC=tE->textCursor();
  tC.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor, 1);
  tC.select(QTextCursor::WordUnderCursor);
  tE->setTextCursor(tC);
  QObject::blockSignals(false);
  ui.imageSizeSpinBox->blockSignals(false);
  ui.textEdit->blockSignals(false);

  ui.imageSizeSpinBox->setFocus();
  
  

}
