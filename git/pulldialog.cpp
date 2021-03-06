/******************************************************************************
 *   Copyright (C) 2010 by Sebastian Doerner <sebastian@sebastian-doerner.de> *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA               *
 ******************************************************************************/

#include "pulldialog.h"
#include "gitwrapper.h"

#include <kcombobox.h>
#include <klocale.h>

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

PullDialog::PullDialog(QWidget* parent):
    KDialog(parent, Qt::Dialog)
{
    this->setCaption(xi18nc("@title:window", "<application>Git</application> Pull"));
    this->setButtons(KDialog::Ok | KDialog::Cancel);
    this->setDefaultButton(KDialog::Ok);
    this->setButtonText(KDialog::Ok, i18nc("@action:button", "Pull"));

    QWidget * boxWidget = new QWidget(this);
    QVBoxLayout * boxLayout = new QVBoxLayout(boxWidget);
    this->setMainWidget(boxWidget);

    QGroupBox * sourceGroupBox = new QGroupBox(boxWidget);
    boxLayout->addWidget(sourceGroupBox);
    sourceGroupBox->setTitle(i18nc("@title:group The source to pull from", "Source"));
    QHBoxLayout * sourceHBox = new QHBoxLayout(sourceGroupBox);
    sourceGroupBox->setLayout(sourceHBox);

    QLabel * remoteLabel = new QLabel(i18nc("@label:listbox a git remote", "Remote:"), sourceGroupBox);
    sourceHBox->addWidget(remoteLabel);
    m_remoteComboBox = new KComboBox(false, sourceGroupBox);
    sourceHBox->addWidget(m_remoteComboBox);

    QLabel * remoteBranchLabel = new QLabel(i18nc("@label:listbox", "Remote branch:"), sourceGroupBox);
    sourceHBox->addWidget(remoteBranchLabel);
    m_remoteBranchComboBox = new KComboBox(false, sourceGroupBox);
    sourceHBox->addWidget(m_remoteBranchComboBox);

    //populate UI
    GitWrapper * gitWrapper = GitWrapper::instance();

    //get sources
    m_remoteComboBox->addItems(gitWrapper->pullRemotes());

    //get branch names
    int currentBranchIndex;
    QStringList branches = gitWrapper->branches(&currentBranchIndex);

    foreach (const QString& branch, branches) {
        if (branch.startsWith(QLatin1String("remotes/"))) {
            const QString remote = branch.section('/', 1, 1);
            const QString name = branch.section('/', 2);
            m_remoteBranches[remote] << name;
        }
    }
    remoteSelectionChanged(m_remoteComboBox->currentText());

    //Signals
    connect(m_remoteComboBox, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(remoteSelectionChanged(QString)));
}

QString PullDialog::source() const
{
    return m_remoteComboBox->currentText();
}

QString PullDialog::remoteBranch() const
{
    return m_remoteBranchComboBox->currentText();
}

void PullDialog::remoteSelectionChanged(const QString& newRemote)
{
    m_remoteBranchComboBox->clear();
    m_remoteBranchComboBox->addItems(m_remoteBranches.value(newRemote));
    this->enableButtonOk(m_remoteBranchComboBox->count() > 0);
}

#include "pulldialog.moc"
