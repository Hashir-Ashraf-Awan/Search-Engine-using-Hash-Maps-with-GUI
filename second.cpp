#include "second.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextCursor>
#include <QTextBlock>
#include <QFileInfo>

FileContentDialog::FileContentDialog(const QString &filePath, QWidget *parent)
    : QDialog(parent), textEdit(new QTextEdit(this)), layout(new QVBoxLayout(this))
{
    layout->addWidget(textEdit);
    setLayout(layout);

    textEdit->setReadOnly(true);
    loadFileContent(filePath);
}

void FileContentDialog::loadFileContent(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Error", "Cannot open file: " + filePath);
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    textEdit->setPlainText(content);
    addFileReferenceButtons(content);
    file.close();
}

void FileContentDialog::addFileReferenceButtons(const QString &content)
{
    QRegularExpression regex(R"((([a-zA-Z]:)?[^\s]*\.(txt|pdf|docx|doc)))");
    QRegularExpressionMatchIterator i = regex.globalMatch(content);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString fileReference = match.captured(1);
        QFileInfo fileInfo(fileReference);
        if (fileInfo.exists()) {
            QPushButton *fileButton = new QPushButton(fileReference, this);
            connect(fileButton, &QPushButton::clicked, this, &FileContentDialog::onFileReferenceClicked);
            layout->addWidget(fileButton);
        }
    }
}

void FileContentDialog::onFileReferenceClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button)
    {
        QString filePath = button->text();
        FileContentDialog *dialog = new FileContentDialog(filePath, this);
        dialog->exec();
    }
}
