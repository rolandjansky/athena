#include "QatPlotWidgets/LabelEditorDialog.h"
#include <QColorDialog>
#include <iostream>
#include "ui_LabelEditorDialog.h"

class LabelEditorDialog::Clockwork {
public:
  Ui::LabelEditorDialog ui;
  CharacterWidget *cw;
};

LabelEditorDialog::~LabelEditorDialog() {
  delete m_c;
}
LabelEditorDialog::LabelEditorDialog(QWidget *parent): QDialog(parent) ,m_c(new Clockwork()){
  m_c->ui.setupUi(this);
  connect (m_c->ui.boldButton,         SIGNAL(clicked()), this, SLOT(textBold()));
  connect (m_c->ui.italicButton,       SIGNAL(clicked()), this, SLOT(textItalic()));
  connect (m_c->ui.scriptButton,       SIGNAL(clicked()), this, SLOT(textScript()));
  connect (m_c->ui.fontComboBox,       SIGNAL(activated( const QString & ) ), this, SLOT(textFamily(const QString &)));
  connect (m_c->ui.fontSizeSpinBox,    SIGNAL(valueChanged(int)),             this, SLOT(textSize(int)));
  connect (m_c->ui.colorButton,        SIGNAL(clicked()),                     this, SLOT(textColor()));        
  connect (m_c->ui.specialCButton,     SIGNAL(clicked()),                     this, SLOT(toggleChars()));
  QColor color("black");

  QPalette palette = m_c->ui.colorButton->palette();
  palette.setColor(QPalette::Button,color);
  m_c->ui.colorButton->setAutoFillBackground(true);
  m_c->ui.colorButton->setPalette(palette);

  m_c->cw=new CharacterWidget(0);
  m_c->ui.frame->setWidget(m_c->cw);
  connect (m_c->ui.fontComboBox,       SIGNAL(activated( const QString &)),   m_c->cw,   SLOT(updateFamily(const QString & )));
  connect (m_c->cw, SIGNAL(characterSelected(const QString &)), m_c->ui.textEdit, SLOT(insertPlainText(const QString & )));
  
  m_c->cw->updateSize("12");

  connect(m_c->ui.textEdit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)), this, SLOT(updateCharFormat(const QTextCharFormat &)));
  m_c->ui.frame->hide();
  setMinimumSize(600,200);
  resize(minimumSize());

  QTextCharFormat format;
  QFont font;
  font.setFamily("Arial");
  font.setPointSize(24);
  format.setFont(font);
  m_c->ui.textEdit->setCurrentCharFormat(format);
  m_c->ui.textEdit->setFocus();
  m_c->ui.textEdit->setText(QString(""));
}

const QTextEdit *LabelEditorDialog::textEdit() const {
  return m_c->ui.textEdit;
}


QTextEdit *LabelEditorDialog::textEdit() {
  return m_c->ui.textEdit;
}




void LabelEditorDialog::textFamily( const QString &f )
{
  QTextEdit * tE = m_c->ui.textEdit;
  QTextCharFormat format; 
  format.setFontFamily(f);
  tE->mergeCurrentCharFormat(format);
}


void LabelEditorDialog::textSize( int i )
{
  QTextEdit * tE = m_c->ui.textEdit;
  QTextCharFormat format; 
  format.setFontPointSize(i);
  tE->mergeCurrentCharFormat(format);
}


void LabelEditorDialog::textBold()
{
 
  QFont font=m_c->ui.boldButton->font();
  bool flag=font.bold();
  flag = !flag;
  font.setBold(flag);
  m_c->ui.boldButton->setFont(font);


  QTextEdit * tE = m_c->ui.textEdit;
  QTextCharFormat format; 
  format.setFontWeight(flag ?  QFont::Bold : QFont::Normal );
  tE->mergeCurrentCharFormat(format);

}

void LabelEditorDialog::textItalic()
{
  QFont font=m_c->ui.italicButton->font();
  bool flag=font.italic();
  flag = !flag;
  font.setItalic(flag);
  m_c->ui.italicButton->setFont(font);


  QTextEdit * tE = m_c->ui.textEdit;
  QTextCharFormat format; 
  format.setFontItalic(flag);
  tE->mergeCurrentCharFormat(format);
}


void LabelEditorDialog::textScript()
{
  QTextEdit * tE = m_c->ui.textEdit;
  QTextCharFormat format;

  QString  text = m_c->ui.scriptButton->text();
  if (text==QString("-")) {
    m_c->ui.scriptButton->setText(QString("^"));
    format.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
  }
  if (text==QString("^")){
    m_c->ui.scriptButton->setText(QString("_"));  
    format.setVerticalAlignment(QTextCharFormat::AlignSubScript);
  } 
  if (text==QString("_")) {
    m_c->ui.scriptButton->setText(QString("-"));
    format.setVerticalAlignment(QTextCharFormat::AlignNormal);
  }
  
  tE->mergeCurrentCharFormat(format);
}


void LabelEditorDialog::textColor()
{
  QColor textColorVal;
  QColor col = QColorDialog::getColor(textColorVal, this );
  if ( !col.isValid() ) return;

  QPalette palette = m_c->ui.colorButton->palette();
  palette.setColor(QPalette::Button,col);
  m_c->ui.colorButton->setAutoFillBackground(true);
  m_c->ui.colorButton->setPalette(palette);

  QTextEdit * tE = m_c->ui.textEdit;
  tE->setTextColor(col);
}

void LabelEditorDialog::updateCharFormat(const QTextCharFormat &f) {
  
  // Don't update if cursor moves to select text, it is damn annoying.
  if (m_c->ui.textEdit->textCursor().selectionStart()!=m_c->ui.textEdit->textCursor().selectionEnd()) return;

  static QTextCharFormat df;
  df.setVerticalAlignment(QTextCharFormat::AlignNormal);
  df.setFontItalic(false);
  df.setFontWeight(QFont::Normal);
  df.setFontPointSize(24);
  df.setFontFamily("Arial");

  QTextCharFormat F = f.fontPointSize()!=0.0 ? f: df;
  if (F.verticalAlignment()==QTextCharFormat::AlignNormal)     m_c->ui.scriptButton->setText(QString("-"));
  if (F.verticalAlignment()==QTextCharFormat::AlignSubScript)  m_c->ui.scriptButton->setText(QString("_"));
  if (F.verticalAlignment()==QTextCharFormat::AlignSuperScript)  m_c->ui.scriptButton->setText(QString("^"));
  {
    QFont font=m_c->ui.italicButton->font(); 
    font.setItalic (F.fontItalic()); 
    m_c->ui.italicButton->setFont(font); 
  }
  {
    QFont font=m_c->ui.boldButton->font(); 
    font.setBold(F.fontWeight()==QFont::Bold);
    m_c->ui.boldButton->setFont(font); 
  }

  m_c->ui.fontSizeSpinBox->setValue(int(F.fontPointSize()+0.5));

  QString fam=F.fontFamily();
  m_c->ui.fontComboBox->setEditText(fam);

  
  QColor color=F.foreground().color();
  QPalette palette = m_c->ui.colorButton->palette();
  palette.setColor(QPalette::Button,color);
  m_c->ui.colorButton->setAutoFillBackground(true);
  m_c->ui.colorButton->setPalette(palette);
}



void LabelEditorDialog::toggleChars() {
  if (m_c->ui.frame->isHidden()) {
    m_c->ui.frame->show();
    setMinimumSize(600,600);
    resize(minimumSize());
  }
  else {
    
    m_c->ui.frame->hide();
    setMinimumSize(600,200);
    resize(minimumSize());
  }

}

