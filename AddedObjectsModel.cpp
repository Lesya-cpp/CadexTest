#include "AddedObjectsModel.hpp"
#include <QMetaEnum>
#include <QMetaObject>
#include <QCoreApplication>

namespace {
  const QMetaEnum figureTypeMetaEnum = QMetaEnum::fromType<FigureData::Type>();
}

AddedObjectsModel::AddedObjectsModel():
  lockFigures(std::make_unique<QReadWriteLock>(QReadWriteLock::Recursive))
{ }

AddedObjectsModel::AddedObjectsModel(std::vector<std::unique_ptr<FigureData>> &&figures):
  figures(std::move(figures)),
  lockFigures(std::make_unique<QReadWriteLock>(QReadWriteLock::Recursive))
{
  for(const auto &el: figures)
    el->setParent(this);
}

int AddedObjectsModel::rowCount(const QModelIndex &parent) const
{
  QReadLocker l(lockFigures.get());
  return figures.size();
}

QVariant AddedObjectsModel::data(const QModelIndex &index, int role) const
{
  if (index.column() != 0 || index.row() >= figures.size())
    return QVariant::Invalid;

  QReadLocker l(lockFigures.get());
  switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
      return QVariant::fromValue(figures.at(index.row()).get());

    case TypeRole:
      return QVariant::fromValue(figures.at(index.row())->getType());
    case TypeStrRole:
      return figureTypeMetaEnum.valueToKey(static_cast<int>(figures.at(index.row())->getType()));
    case CountRole:
      return figures.at(index.row())->getCount();
    case ScaleXRole:
      return figures.at(index.row())->getScaleX();
    case ScaleYRole:
      return figures.at(index.row())->getScaleY();
    case ScaleZRole:
      return figures.at(index.row())->getScaleZ();
    default:
      return QVariant::Invalid;
    }
}

bool AddedObjectsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (index.column() != 0 || index.row() >= figures.size())
    return false;

  QReadLocker l(lockFigures.get());
  switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
      if (auto figure = qobject_cast<FigureData*>(qvariant_cast<QObject*>(value)); figure)
        {
          figures[index.row()].reset(figure);
          emit dataChanged(index, index, {role});
          return true;
        } else
        return false;

    case TypeRole:
      if (value.canConvert<FigureData::Type>())
        {
          figures.at(index.row())->setType(qvariant_cast<FigureData::Type>(value));
          emit dataChanged(index, index, {role});
          return true;
        } else
        return false;

    case TypeStrRole:
      if (value.canConvert<QString>())
        {
          bool ok;
          figures.at(index.row())->setType(
                static_cast<FigureData::Type>(figureTypeMetaEnum.keyToValue(value.toString().toLocal8Bit().data(), &ok))
                );
          if(ok)
            emit dataChanged(index, index, {role});
          return ok;
        } else
        return false;

    case CountRole:
      if (value.canConvert<int>())
        {
          figures.at(index.row())->setCount(qvariant_cast<int>(value));
          emit dataChanged(index, index, {role});
          return true;
        } else
        return false;

    case ScaleXRole:
      if (value.canConvert<float>())
        {
          figures.at(index.row())->setScaleX(qvariant_cast<float>(value));
          emit dataChanged(index, index, {role});
          return true;
        } else
        return false;

    case ScaleYRole:
      if (value.canConvert<float>())
        {
          figures.at(index.row())->setScaleY(qvariant_cast<float>(value));
          emit dataChanged(index, index, {role});
          return true;
        } else
        return false;

    case ScaleZRole:
      if (value.canConvert<float>())
        {
          figures.at(index.row())->setScaleZ(qvariant_cast<float>(value));
          emit dataChanged(index, index, {role});
          return true;
        } else
        return false;
    default:
      return false;
    }
  return false;
}

QHash<int, QByteArray> AddedObjectsModel::roleNames() const
{
  static const QHash<int, QByteArray> roleNames = {
    { Qt::DisplayRole, "display" },
    { Qt::DecorationRole, "decoration" },
    { Qt::EditRole, "edit" },
    { Qt::ToolTipRole, "toolTip" },
    { Qt::StatusTipRole, "statusTip" },
    { Qt::WhatsThisRole, "whatsThis" },
    { TypeRole, "type" },
    { TypeStrRole, "figure_type" },
    { CountRole, "figures_count" },
    { ScaleXRole, "scale_x" },
    { ScaleYRole, "scale_y" },
    { ScaleZRole, "scale_z" },
  };
  return roleNames;
}

void AddedObjectsModel::addObject(int type, int count, qreal scaleX, qreal scaleY, qreal scaleZ)
{
  QWriteLocker l(lockFigures.get());
  beginInsertColumns(QModelIndex(), figures.size(), figures.size());
  figures.push_back(std::make_unique<FigureData>(static_cast<FigureData::Type>(type), count, QVector3D(scaleX, scaleY, scaleZ)));
  figures.back()->setParent(this);
  l.unlock();
  QMetaObject::invokeMethod(QCoreApplication::instance(), [this] {
      endInsertRows();
    }, Qt::BlockingQueuedConnection);
}

void AddedObjectsModel::removeObject(int index)
{
  QWriteLocker l(lockFigures.get());
  beginRemoveRows(QModelIndex(), index, index);
  if (index < 0 || index >= figures.size())
    return;
  figures.erase(figures.begin()+index);
  l.unlock();
  QMetaObject::invokeMethod(QCoreApplication::instance(), [this] {
      endRemoveRows();
    }, Qt::BlockingQueuedConnection);
}
