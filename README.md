SimplePack (SPK)
===============

The spk archiving format is a developer-friendly(read: easy to use, even without libspk) for archiving files and directories.

File format
-----------
Magic Value: 'S' 'P' 'K' '\xFF' '\0'

Repeating:
<table>
    <tr> <th>Name</th>              <th>Description</th>                                  <th>Size</th>             </tr>
    <tr> <td>Path</td>              <td>Contains the path, padded with 0x00</td>          <td>char[255]</td>        </tr>
    <tr> <td>Type</td>              <td>Contains the type: 1 = file; 2 = directory</td>   <td>uint8_t</td>          </tr>
    <tr> <td>Mode</td>              <td>File access permissions</td>                      <td>uint16_t</td>         </tr>
    <tr> <td>UID</td>               <td>User ID for file/directory</td>                   <td>uint16_t</td>         </tr>
    <tr> <td>GID</td>               <td>Group ID for file/directory</td>                  <td>uint16_t</td>         </tr>
    <tr> <td>Length</td>            <td>Size of archived file, always 0 if directory</td> <td>uint32_t</td>         </tr>
    <tr> <td>Content</td>           <td>file contents</td>                                <td><i>$Length</i></td>   </tr>
</table>

