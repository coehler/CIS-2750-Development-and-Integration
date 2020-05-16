# CIS 2750 Development and Integration

This repo contains the completed term project for CIS 2750 Software Systems Development and Integration, 2018. CIS 2750 is generally considered to be the hardest course in the University of Guelph Computer Science program.

## Description

The 2018 project called for students to create a three-tier client/server/database web application that can parse, display, and store GEDCOM, a data exchange format used for genealogical data. Examples of GEDCOM can be viewed in the `uploads` directory.

The backend of the project is primary composed of a simplified GEDCOM parser, which was written from scratch in C99. It can be viewed in the `parser` directory.

The web portion of the of the project uses Express.js and Ajax to handle web requests to parse and display GEDCOM files. It can be view in the `public` directory and in `app.js`.