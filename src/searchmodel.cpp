/*
    Copyright (C) 2017 Sebastian J. Wolf

    This file is part of Piepmatz.

    Piepmatz is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Piepmatz is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Piepmatz. If not, see <http://www.gnu.org/licenses/>.
*/
#include "searchmodel.h"

SearchModel::SearchModel(TwitterApi *twitterApi)
    : searchInProgress(false)
{
    this->twitterApi = twitterApi;

    connect(twitterApi, &TwitterApi::searchTweetsError, this, &SearchModel::handleSearchTweetsError);
    connect(twitterApi, &TwitterApi::searchTweetsSuccessful, this, &SearchModel::handleSearchTweetsSuccessful);
}

int SearchModel::rowCount(const QModelIndex &) const
{
    return searchResults.size();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(searchResults.value(index.row()));
    }
    return QVariant();
}

void SearchModel::search(const QString &query)
{
    searchInProgress = true;
    QRegExp regex("(\\w+)");
    if (query.contains(regex) || query.isEmpty()) {
        twitterApi->searchTweets(query);
    } else {
        searchInProgress = false;
        emit searchFinished();
    }
}

void SearchModel::handleSearchTweetsSuccessful(const QVariantList &result)
{
    qDebug() << "SearchModel::handleSearchTweetsSuccessful";
    qDebug() << "Result Count: " << QString::number(result.length());

    if (searchInProgress) {
        beginResetModel();
        searchResults.clear();
        searchResults.append(result);
        endResetModel();
        searchInProgress = false;
        emit searchFinished();
    } else {
        qDebug() << "Search API called from somewhere else...";
    }

}

void SearchModel::handleSearchTweetsError(const QString &errorMessage)
{
    if (searchInProgress) {
        searchInProgress = false;
        emit searchError(errorMessage);
    } else {
        qDebug() << "Search API called from somewhere else...";
    }
}
