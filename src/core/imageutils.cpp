/*
 * Strawberry Music Player
 * Copyright 2019, Jonas Kvinge <jonas@jkvinge.net>
 *
 * Strawberry is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Strawberry is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Strawberry.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "config.h"

#include <QList>
#include <QBuffer>
#include <QVariant>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QImage>
#include <QImageReader>
#include <QPixmap>
#include <QPainter>
#include <QSize>
#include <QSettings>

#include "imageutils.h"
#include "core/utilities.h"
#include "core/tagreaderclient.h"

QStringList ImageUtils::kSupportedImageMimeTypes;
QStringList ImageUtils::kSupportedImageFormats;

QStringList ImageUtils::SupportedImageMimeTypes() {

  if (kSupportedImageMimeTypes.isEmpty()) {
    for (const QByteArray &mimetype : QImageReader::supportedMimeTypes()) {
      kSupportedImageMimeTypes << mimetype;
    }
  }

  return kSupportedImageMimeTypes;

}

QStringList ImageUtils::SupportedImageFormats() {

  if (kSupportedImageFormats.isEmpty()) {
    for (const QByteArray &filetype : QImageReader::supportedImageFormats()) {
      kSupportedImageFormats << filetype;
    }
  }

  return kSupportedImageFormats;

}

QList<QByteArray> ImageUtils::ImageFormatsForMimeType(const QByteArray &mimetype) {

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
  return QImageReader::imageFormatsForMimeType(mimetype);
#else
  if (mimetype == "image/bmp") return QList<QByteArray>() << "BMP";
  else if (mimetype == "image/gif") return QList<QByteArray>() << "GIF";
  else if (mimetype == "image/jpeg") return QList<QByteArray>() << "JPG";
  else if (mimetype == "image/png") return QList<QByteArray>() << "PNG";
  else return QList<QByteArray>();
#endif

}

QPixmap ImageUtils::TryLoadPixmap(const QUrl &art_automatic, const QUrl &art_manual, const QUrl &url) {

  QPixmap ret;

  if (!art_manual.path().isEmpty()) {
    if (art_manual.path() == Song::kManuallyUnsetCover) return ret;
    else if (art_manual.isLocalFile()) {
      ret.load(art_manual.toLocalFile());
    }
    else if (art_manual.scheme().isEmpty()) {
      ret.load(art_manual.path());
    }
  }
  if (ret.isNull() && !art_automatic.path().isEmpty()) {
    if (art_automatic.path() == Song::kEmbeddedCover && !url.isEmpty() && url.isLocalFile()) {
      ret = QPixmap::fromImage(TagReaderClient::Instance()->LoadEmbeddedArtAsImageBlocking(url.toLocalFile()));
    }
    else if (art_automatic.isLocalFile()) {
      ret.load(art_automatic.toLocalFile());
    }
    else if (art_automatic.scheme().isEmpty()) {
      ret.load(art_automatic.path());
    }
  }

  return ret;

}

QByteArray ImageUtils::SaveImageToJpegData(const QImage &image) {

  if (image.isNull()) return QByteArray();

  QByteArray image_data;
  QBuffer buffer(&image_data);
  if (buffer.open(QIODevice::WriteOnly)) {
    image.save(&buffer, "JPEG");
    buffer.close();
  }

  return image_data;

}

QByteArray ImageUtils::FileToJpegData(const QString &filename) {

  if (filename.isEmpty()) return QByteArray();

  QByteArray image_data = Utilities::ReadDataFromFile(filename);
  if (Utilities::MimeTypeFromData(image_data) == "image/jpeg") return image_data;
  else {
    QImage image;
    if (image.loadFromData(image_data)) {
      if (!image.isNull()) {
        image_data = SaveImageToJpegData(image);
      }
    }
  }

  return image_data;

}

QImage ImageUtils::ScaleAndPad(const QImage &image, const bool scale, const bool pad, const int desired_height) {

  if (image.isNull()) return image;

  // Scale the image down
  QImage image_scaled;
  if (scale) {
    image_scaled = image.scaled(QSize(desired_height, desired_height), Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }
  else {
    image_scaled = image;
  }

  // Pad the image to height x height
  if (pad) {
    QImage image_padded(desired_height, desired_height, QImage::Format_ARGB32);
    image_padded.fill(0);

    QPainter p(&image_padded);
    p.drawImage((desired_height - image_scaled.width()) / 2, (desired_height - image_scaled.height()) / 2, image_scaled);
    p.end();

    image_scaled = image_padded;
  }

  return image_scaled;

}

QImage ImageUtils::CreateThumbnail(const QImage &image, const bool pad, const QSize size) {

  if (image.isNull()) return image;

  QImage image_thumbnail;
  if (pad) {
    image_thumbnail = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QImage image_padded(size, QImage::Format_ARGB32_Premultiplied);
    image_padded.fill(0);

    QPainter p(&image_padded);
    p.drawImage((image_padded.width() - image_thumbnail.width()) / 2, (image_padded.height() - image_thumbnail.height()) / 2, image_thumbnail);
    p.end();

    image_thumbnail = image_padded;
  }
  else {
    image_thumbnail = image.scaledToHeight(size.height(), Qt::SmoothTransformation);
  }

  return image_thumbnail;

}

QImage ImageUtils::GenerateNoCoverImage(const QSize size) {

  QImage image(":/pictures/cdcase.png");

  // Get a square version of the nocover image with some transparency:
  QImage image_scaled = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  QImage image_square(size, QImage::Format_ARGB32);
  image_square.fill(0);
  QPainter p(&image_square);
  p.setOpacity(0.4);
  p.drawImage((size.width() - image_scaled.width()) / 2, (size.height() - image_scaled.height()) / 2, image_scaled);
  p.end();

  return image_square;

}
