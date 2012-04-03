/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}

*/

#include "WaypointDetailsReader.hpp"
#include "Language/Language.hpp"
#include "Profile/ProfileKeys.hpp"
#include "LogFile.hpp"
#include "Interface.hpp"
#include "Util/StringUtil.hpp"
#include "Engine/Waypoint/Waypoint.hpp"
#include "Engine/Waypoint/Waypoints.hpp"
#include "IO/ConfiguredFile.hpp"
#include "Operation/Operation.hpp"

#include <vector>

static const Waypoint *
find_waypoint(Waypoints &way_points, const TCHAR *name)
{
  const Waypoint *wp = way_points.LookupName(name);
  if (wp != NULL)
    return wp;

  // TODO: Comments please! What is this supposed to do? Why do we need it?
  size_t name_length = _tcslen(name);
  TCHAR buffer[name_length + 4];
  _tcscpy(buffer, name);
  _tcscpy(buffer + name_length, _T(" AF"));
  wp = way_points.LookupName(buffer);
  if (wp != NULL)
    return wp;

  _tcscpy(buffer + name_length, _T(" AD"));
  wp = way_points.LookupName(buffer);
  if (wp != NULL)
    return wp;

  return NULL;
}

static void
SetAirfieldDetails(Waypoints &way_points, const TCHAR *name,
                   const tstring &Details,
                   const std::vector<tstring> &files_external,
                   const std::vector<tstring> &files_embed)
{
  const Waypoint *wp = find_waypoint(way_points, name);
  if (wp == NULL)
    return;

  Waypoint new_wp(*wp);
  new_wp.details = Details.c_str();
  new_wp.files_embed.assign(files_embed.begin(), files_embed.end());
#ifdef ANDROID
  new_wp.files_external.assign(files_external.begin(), files_external.end());
#endif
  way_points.Replace(*wp, new_wp);
  way_points.Optimise();
}

/**
 * Parses the data provided by the airfield details file handle
 */
static void
ParseAirfieldDetails(Waypoints &way_points, TLineReader &reader,
                     OperationEnvironment &operation)
{
  tstring Details;
  std::vector<tstring> files_external, files_embed;
  TCHAR Name[201];
  const TCHAR *filename;

  Name[0] = 0;

  bool inDetails = false;
  int i;

  long filesize = std::max(reader.size(), 1l);
  operation.SetProgressRange(100);

  TCHAR *TempString;
  while ((TempString = reader.read()) != NULL) {
    if (TempString[0] == _T('[')) { // Look for start
      if (inDetails)
        SetAirfieldDetails(way_points, Name, Details, files_external,
                           files_embed);

      Details.clear();
      files_external.clear();
      files_embed.clear();

      // extract name
      for (i = 1; i < 201; i++) {
        if (TempString[i] == _T(']'))
          break;

        Name[i - 1] = TempString[i];
      }
      Name[i - 1] = 0;

      inDetails = true;

      operation.SetProgressPosition(reader.tell() * 100 / filesize);
    } else if ((filename =
                StringAfterPrefixCI(TempString, _T("image="))) != NULL) {
      files_embed.push_back(filename);
    } else if ((filename =
                StringAfterPrefixCI(TempString, _T("file="))) != NULL) {
#ifdef ANDROID
      files_external.push_back(filename);
#endif
    } else {
      // append text to details string
      if (!StringIsEmpty(TempString)) {
        Details += TempString;
        Details += _T('\n');
      }
    }
  }

  if (inDetails)
    SetAirfieldDetails(way_points, Name, Details, files_external, files_embed);
}

/**
 * Opens the airfield details file and parses it
 */
void
WaypointDetails::ReadFile(TLineReader &reader, Waypoints &way_points,
                          OperationEnvironment &operation)
{
  LogStartUp(_T("WaypointDetails::ReadFile"));
  operation.SetText(_("Loading Airfield Details File..."));
  ParseAirfieldDetails(way_points, reader, operation);
}

void
WaypointDetails::ReadFileFromProfile(Waypoints &way_points,
                                     OperationEnvironment &operation)
{
  LogStartUp(_T("WaypointDetails::ReadFileFromProfile"));

  TLineReader *reader =
    OpenConfiguredTextFile(szProfileAirfieldFile, _T("airfields.txt"),
                           ConvertLineReader::AUTO);
  if (reader == NULL)
    return;

  ReadFile(*reader, way_points, operation);
  delete reader;
}
