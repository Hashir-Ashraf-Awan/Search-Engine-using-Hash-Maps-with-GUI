#ifndef SECOND_H
#define SECOND_H


#include <QDialog>
#include <QTextEdit>
#include <QString>
#include <QPushButton>
#include <QVBoxLayout>

class FileContentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileContentDialog(const QString &filePath, QWidget *parent = nullptr);

private slots:
    void onFileReferenceClicked();

private:
    void loadFileContent(const QString &filePath);
    void addFileReferenceButtons(const QString &content);

    QTextEdit *textEdit;
    QVBoxLayout *layout;
};

#endif // FILECONTENTDIALOG_H
