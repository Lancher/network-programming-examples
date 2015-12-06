/*  Copyright (C) 2015  Steve (unicornonair@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// A Simple example of "struct iovec" to print strings.

#include <stdio.h>
#include <stdlib.h>

#include <sys/uio.h>           // struct iovec, readv(), writev()

int
main(int argc, char *argv[])
{
  char str2[] = "This is mango speaking";
  char str3[] = "!!!!";
  char str1[] = "Hello, ";
  struct iovec iov[3];

  iov[0].iov_base = str1;
  iov[0].iov_len = sizeof (str1);

  iov[1].iov_base = str2;
  iov[1].iov_len = sizeof (str2);

  iov[2].iov_base = str3;
  iov[2].iov_len = sizeof (str3);

  // Write the io vector to the stdout.
  writev (1, iov, 3);

  exit (EXIT_SUCCESS);
}