#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QUrlQuery>
#include <random>

int retryCount1=0;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    manager = new QNetworkAccessManager(this);
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);   //start window

    ui->label_3->setText("<img src='file:///C:/Users/Hp/Downloads/_9edc6383-c2a7-4159-b5e2-d4e379bd2e6b' width='290' height='180'>");

    connect(ui->Play_Button, &QPushButton::clicked, [this]() {
        ui->stackedWidget->setCurrentIndex(1);
    });

    // Connect Language Buttons for fetching word and translating
    connect(ui->Spanish_Button, &QPushButton::clicked, [this]() {
        selectedLanguage = "Spanish";
        ui->stackedWidget->setCurrentIndex(2);
        displayWordOfTheDay("es",retryCount1);
    });

    connect(ui->Catalan_Button, &QPushButton::clicked, [this]() {
        selectedLanguage = "Catalan";
        ui->stackedWidget->setCurrentIndex(2);
        displayWordOfTheDay("ca",retryCount1);
    });

    connect(ui->Galician_Button, &QPushButton::clicked, [this]() {
        selectedLanguage = "Galician";
        ui->stackedWidget->setCurrentIndex(2);
        displayWordOfTheDay("gl",retryCount1);
    });

    connect(ui->Quiz_Button, &QPushButton::clicked, [this]() {
        ui->stackedWidget->setCurrentIndex(3);
        qDebug() << "Flashcards Button clicked check";
        startQuiz();
    });

    connect(ui->SerboCroatian_Button, &QPushButton::clicked, [this]() {
        selectedLanguage = "Serbo-Croatian";
        ui->stackedWidget->setCurrentIndex(2);
        displayWordOfTheDay("sr",retryCount1);
    });

    connect(ui->quizNextButton, &QPushButton::clicked, this, &MainWindow::on_quizNextButton_clicked);
    connect(ui->endQuizButton, &QPushButton::clicked, this, &MainWindow::onEndQuizButtonClicked);
    connect(ui->Flashcards_Button, &QPushButton::clicked, [this]() {
        ui->stackedWidget->setCurrentIndex(5);
        displayFlashcard();
    });

    for (int i = 0; i < 5; ++i) {
        foreignWords[i] = "";
        englishTranslations[i] = "";
    }

    fetchFlashcards();

    // Connectting buttons
    connect(ui->flipButton, &QPushButton::pressed, this, &MainWindow::onFlipButtonPressed);
    connect(ui->flipButton, &QPushButton::released, this, &MainWindow::onFlipButtonReleased);
    connect(ui->nextFlashcardButton, &QPushButton::clicked, this, &MainWindow::onNextFlashcardClicked);

    connect(ui->BackButton1, &QPushButton::clicked, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    connect(ui->BackButton2, &QPushButton::clicked, [this]() {
        ui->stackedWidget->setCurrentIndex(1);
    });

    connect(ui->BackButton3, &QPushButton::clicked, [this]() {
        ui->stackedWidget->setCurrentIndex(2);
    });

    connect(ui->BackButton4, &QPushButton::clicked, [this]() {
        ui->stackedWidget->setCurrentIndex(2);
    });

}

MainWindow::~MainWindow() {
    delete ui;
}

QString TranslateWord = "";
QString wordOfTheDaySpanish = "";
QString wordOfTheDayCatalan = "";
QString wordOfTheDayGalacian = "";
QString wordOfTheDaySerboCroatian = "";
int retryCount=0;


void MainWindow::displayWordOfTheDay(const QString& langCode, int retryCount1) {
    const int maxRetries = 20;
    QUrl url("https://random-word-api.vercel.app/api?words=1");

    QNetworkRequest request(url);


    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, langCode, retryCount1, maxRetries]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

            if (jsonDoc.isArray()) {
                QJsonArray jsonArray = jsonDoc.array();
                if (!jsonArray.isEmpty()) {
                    QString wordOfTheDay = jsonArray[0].toString();
                    TranslateWord = wordOfTheDay;


                    if (langCode == "es") {
                        wordOfTheDaySpanish = wordOfTheDay;
                         ui->label_2->setText("<img src='file:///C:/Users/Hp/Downloads/spain.png' width='100' height='50'>");
                    } else if (langCode == "ca") {
                        wordOfTheDayCatalan = wordOfTheDay;
                         ui->label_2->setText("<img src='file:///C:/Users/Hp/Downloads/catalan.png' width='100' height='50'>");
                    } else if (langCode == "gl") {
                        wordOfTheDayGalacian = wordOfTheDay;
                        ui->label_2->setText("<img src='file:///C:/Users/Hp/Downloads/galician.png' width='100' height='50'>");
                    } else if (langCode == "sr") {
                        wordOfTheDaySerboCroatian = wordOfTheDay;
                        ui->label_2->setText("<img src='file:///C:/Users/Hp/Downloads/croatia.png' width='100' height='50'>");
                    }

                    // Update the label and proceed to fetch translation
                    ui->label->setText("💡 Word Of The Day");
                    ui->WordOfDay_Label->setText("English translation:  " + wordOfTheDay);

                    fetchTranslation(langCode, retryCount1, maxRetries);
                }
            }
        } else {
            ui->WordOfDay_Label->setText("Error fetching word of the day: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

void MainWindow::fetchTranslation(const QString& langCode, int retryCount1, int maxRetries) {
    QUrl url("http://www.apertium.org/apy/translate");
    QUrlQuery query;

    // Set the appropriate language pair
    if (langCode == "es") {
        query.addQueryItem("langpair", "en|es");
    } else if (langCode == "ca") {
        query.addQueryItem("langpair", "en|ca");
    } else if (langCode == "gl") {
        query.addQueryItem("langpair", "en|gl");
    } else if (langCode == "sr") {
        query.addQueryItem("langpair", "en|sr");
    }

    query.addQueryItem("q", TranslateWord);
    url.setQuery(query);

    QNetworkRequest request(url);

    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, langCode, retryCount1, maxRetries]() {
        handleTranslationReply(reply, langCode, retryCount1, maxRetries);
    });
}

void MainWindow::handleTranslationReply(QNetworkReply* reply, const QString& langCode, int retryCount1, int maxRetries) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

        if (jsonDoc.isObject()) {
            QJsonObject jsonObject = jsonDoc.object();
            QJsonObject responseData = jsonObject["responseData"].toObject();
            QString translatedText = responseData["translatedText"].toString();

            if (!translatedText.isEmpty() && !translatedText.contains('*')) {
                ui->translateLabel->setText("Word: " + translatedText);
            } else if (retryCount1 < maxRetries) {
                qDebug() << "Invalid translation. Retrying with a new word...";
                displayWordOfTheDay(langCode, retryCount1 + 1);
            } else {
                ui->translateLabel->setText("Translation not found after multiple attempts.");
                qDebug() << "Max retries reached for translation.";
            }
        }
    } else {
        qDebug() << "Error fetching translation: " << reply->errorString();
        ui->translateLabel->setText("Error fetching translation.");
    }

    reply->deleteLater();
}

void MainWindow::startQuiz() {
    qDebug() << "Starting the quiz...";
    quizCurrentQuestion = 0;
    quizCorrectAnswers = 0;

    for (int i = 0; i < QUIZ_SIZE; ++i) {
        quizWords[i].clear();
        quizCorrectAnswersArray[i].clear();
        for (int j = 0; j < 4; ++j) {
            quizOptions[i][j].clear();
        }
    }

    fetchQuizWords(0);
}


void MainWindow::fetchQuizWords(int index) {
    if (index >= QUIZ_SIZE) {
        displayQuizQuestion();
        return;
    }

    QUrl url("https://random-word-api.vercel.app/api?words=1");
    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, index]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

            if (jsonDoc.isArray() && !jsonDoc.array().isEmpty()) {
                quizCorrectAnswersArray[index] = jsonDoc.array()[0].toString();
                fetchTranslationForQuiz(quizCorrectAnswersArray[index], index,retryCount);
            } else {
                qDebug() << "Invalid response while fetching quiz word.";
            }
        } else {
            qDebug() << "Error fetching quiz word:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

void MainWindow::fetchTranslationForQuiz(const QString& word, int index, int retryCount) {
    const int maxRetries = 30;

    QUrl url("http://www.apertium.org/apy/translate");
    QUrlQuery query;

    // Update translation direction based on selected language
    query.addQueryItem("langpair", selectedLanguage == "Spanish" ? "en|es" :
                                       selectedLanguage == "Catalan" ? "en|ca" :
                                       selectedLanguage == "Galician" ? "en|gl" : "en|sr");
    if (selectedLanguage == "Spanish") {
        ui->label_4->setText("Spanish");
    }
    else if (selectedLanguage == "Catalan"){
        ui->label_4->setText("Catalan");
    }
    else if (selectedLanguage == "Galician"){
        ui->label_4->setText("Galician");
    }
    else if (selectedLanguage == "Serbo-Croatian"){
        ui->label_4->setText("Serbo-Croatian");
    }
    query.addQueryItem("q", word);
    url.setQuery(query);
    QNetworkRequest request(url);

    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, word, index, retryCount, maxRetries]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

            if (jsonDoc.isObject()) {
                QJsonObject jsonObject = jsonDoc.object();
                QJsonObject responseData = jsonObject["responseData"].toObject();
                QString translatedText = responseData["translatedText"].toString();

                // Check if translation is valid (not the same word with an asterisk)
                if (!translatedText.isEmpty() && !translatedText.contains('*')) {
                    quizWords[index] = translatedText;


                    if (index == QUIZ_SIZE - 1) {
                        displayQuizQuestion();
                    } else {
                        fetchQuizWords(index + 1);
                    }
                } else if (retryCount < maxRetries) {
                    // Retry with a new word if the translation is invalid
                    qDebug() << "Invalid translation for word:" << word << ". Retrying...";
                    fetchQuizWords(index); // Fetch a new word
                } else {
                    qDebug() << "Max retries reached for word:" << word;
                    quizWords[index] = "Translation unavailable"; // Mark as unavailable
                    if (index == QUIZ_SIZE - 1) {
                        displayQuizQuestion();
                    } else {
                        fetchQuizWords(index + 1);
                    }
                }
            }
        } else {
            qDebug() << "Error fetching translation for word:" << word << reply->errorString();
        }
        reply->deleteLater();
    });
}

//This is where I am

void MainWindow::displayQuizQuestion() {

    if (quizCurrentQuestion >= QUIZ_SIZE) {

        ui->stackedWidget->setCurrentIndex(4);  // Assuming index 3 is the results page
        ui->quizFinalScoreLabel->setText(
            QString("Quiz Complete! You got %1 out of %2 correct.")
                .arg(quizCorrectAnswers)
                .arg(QUIZ_SIZE)
            );
        return;
    }

    QString correctAnswer = quizCorrectAnswersArray[quizCurrentQuestion];  // Correct answer for the current question


    QUrl url("https://random-word-api.vercel.app/api?words=4");  // Fetch 4 words
    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, correctAnswer]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
            if (jsonDoc.isArray()) {
                QJsonArray jsonArray = jsonDoc.array();
                QStringList fakeOptions;

                fakeOptions.append(correctAnswer);


                for (int i = 1; i < 4; ++i) {
                    QString fakeOption = jsonArray[i].toString();
                    fakeOptions.append(fakeOption);
                }

                std::random_device rd;
                std::mt19937 g(rd());
                std::shuffle(fakeOptions.begin(), fakeOptions.end(), g);


                ui->quizQuestionLabel->setText("What is the English translation for: " + quizWords[quizCurrentQuestion]);

                ui->quizOption1->setText(fakeOptions[0]);
                ui->quizOption2->setText(fakeOptions[1]);
                ui->quizOption3->setText(fakeOptions[2]);
                ui->quizOption4->setText(fakeOptions[3]);

                ui->quizOption1->setAutoExclusive(false);
                ui->quizOption2->setAutoExclusive(false);
                ui->quizOption3->setAutoExclusive(false);
                ui->quizOption4->setAutoExclusive(false);

                ui->quizOption1->setChecked(false);
                ui->quizOption2->setChecked(false);
                ui->quizOption3->setChecked(false);
                ui->quizOption4->setChecked(false);

                ui->quizOption1->setAutoExclusive(true);
                ui->quizOption2->setAutoExclusive(true);
                ui->quizOption3->setAutoExclusive(true);
                ui->quizOption4->setAutoExclusive(true);
            }
        } else {
            ui->quizFeedbackLabel->setText("Error fetching words: " + reply->errorString());
        }

        reply->deleteLater();
    });
}

void MainWindow::clearRadioButtonSelection() {
    ui->quizOption1->setAutoExclusive(false);
    ui->quizOption2->setAutoExclusive(false);
    ui->quizOption3->setAutoExclusive(false);
    ui->quizOption4->setAutoExclusive(false);

    ui->quizOption1->setChecked(false);
    ui->quizOption2->setChecked(false);
    ui->quizOption3->setChecked(false);
    ui->quizOption4->setChecked(false);

    ui->quizOption1->setAutoExclusive(true);
    ui->quizOption2->setAutoExclusive(true);
    ui->quizOption3->setAutoExclusive(true);
    ui->quizOption4->setAutoExclusive(true);
}

void MainWindow::on_quizNextButton_clicked() {
    QString selectedAnswer;
    if (ui->quizOption1->isChecked()) {
        selectedAnswer = ui->quizOption1->text();
    } else if (ui->quizOption2->isChecked()) {
        selectedAnswer = ui->quizOption2->text();
    } else if (ui->quizOption3->isChecked()) {
        selectedAnswer = ui->quizOption3->text();
    } else if (ui->quizOption4->isChecked()) {
        selectedAnswer = ui->quizOption4->text();
    }

    if (selectedAnswer == quizCorrectAnswersArray[quizCurrentQuestion]) {
        quizCorrectAnswers++;
        ui->quizFeedbackLabel->setText("Correct!");
    } else {
        ui->quizFeedbackLabel->setText(
            QString("Wrong! Correct answer: %1").arg(quizCorrectAnswersArray[quizCurrentQuestion]));
    }

    quizCurrentQuestion++;
    displayQuizQuestion();
}

void MainWindow::onEndQuizButtonClicked() {
    ui->stackedWidget->setCurrentIndex(2);
}

// Fetch flashcards
void MainWindow::fetchFlashcards() {
    for (int i = 0; i < 5; ++i) {
        fetchForeignWord(i);
    }
}

void MainWindow::fetchForeignWord(int index) {
    QUrl url("https://random-word-api.vercel.app/api?words=1");
    QNetworkRequest request(url);

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, index]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

            if (jsonDoc.isArray() && !jsonDoc.array().isEmpty()) {
                QString foreignWord = jsonDoc.array()[0].toString();
                foreignWords[index] = foreignWord;

                // Fetch translation for the fetched word
                fetchTranslation(foreignWord, index);
            }
        } else {
            qDebug() << "Error fetching foreign word:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

void MainWindow::fetchTranslation(const QString &foreignWord, int index) {
    QUrl translateUrl("http://www.apertium.org/apy/translate");
    QUrlQuery query;
    query.addQueryItem("langpair", "en|es");
    query.addQueryItem("q", foreignWord);
    translateUrl.setQuery(query);

    QNetworkReply *reply = manager->get(QNetworkRequest(translateUrl));

    connect(reply, &QNetworkReply::finished, this, [this, reply, index, foreignWord]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

            if (jsonDoc.isObject()) {
                QJsonObject jsonObject = jsonDoc.object();
                QJsonObject responseData = jsonObject["responseData"].toObject();
                QString translatedText = responseData["translatedText"].toString();

                // Check if the translation is valid
                if (!translatedText.startsWith("*") && translatedText != foreignWord) {
                    englishTranslations[index] = translatedText;
                } else {
                    fetchForeignWord(index);
                }
            }
        } else {
            qDebug() << "Error fetching translation:" << reply->errorString();
        }
        reply->deleteLater();
    });
}

void MainWindow::displayFlashcard() {
    if (currentFlashcardIndex < 5) {
        ui->foreignWordLabel->setText(englishTranslations[currentFlashcardIndex]);
        ui->englishWordLabel->setText("");
        searchImagesOnPexels(englishTranslations[currentFlashcardIndex]);
    }
}


void MainWindow::onFlipButtonPressed() {
    if (currentFlashcardIndex < 5) {
        ui->englishWordLabel->setText(foreignWords[currentFlashcardIndex]);
    }
}


void MainWindow::onFlipButtonReleased() {
    ui->englishWordLabel->setText("");
}

void MainWindow::onNextFlashcardClicked() {
    currentFlashcardIndex = (currentFlashcardIndex + 1) % 5; // Loop through 5 flashcards
    displayFlashcard();
}

void MainWindow::searchImagesOnPexels(const QString &query) {
    QString apiKey = "snkRlkYvJLaRDBI15FmZ2qnhEzYuJXaDTPm9Y2o6YkvVje5FYgbwPJkS";  // Replace with your Pexels API Key
    QUrl url("https://api.pexels.com/v1/search");
    QUrlQuery queryParams;
    queryParams.addQueryItem("query", query);
    queryParams.addQueryItem("per_page", "1");

    url.setQuery(queryParams);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", apiKey.toUtf8());

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

            if (jsonDoc.isObject()) {
                QJsonObject jsonObject = jsonDoc.object();
                QJsonArray photosArray = jsonObject["photos"].toArray();

                if (!photosArray.isEmpty()) {
                    QJsonObject firstPhoto = photosArray[0].toObject();
                    QString imageUrl = firstPhoto["src"].toObject()["original"].toString();

                    displayImage(imageUrl);
                }
            }
        } else {
            qDebug() << "Error fetching image: " << reply->errorString();
        }

        reply->deleteLater();
    });
}

void MainWindow::displayImage(const QString &imageUrl) {
    QUrl url(imageUrl);
    QNetworkRequest request(url);

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray imageData = reply->readAll();

            QPixmap pixmap;
            if (pixmap.loadFromData(imageData)) {
               ui->ImageLabel->setPixmap(pixmap.scaled(ui->ImageLabel->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
            } else {
                qDebug() << "Failed to load image";
            }
        } else {
            qDebug() << "Error fetching image: " << reply->errorString();
        }

        reply->deleteLater();
    });
}
