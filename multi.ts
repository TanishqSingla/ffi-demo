import { suffix, dlopen } from "bun:ffi";
import fs from 'fs'

const libpath = `libmetadata.${suffix}`;

const { symbols } = dlopen(libpath, {
	get_metadata: {
		returns: 'cstring'
	},
	add_filename: {
		args: ['cstring']
	},
	print_files: {
	}
});

const files = fs.readdirSync("posts");

files.forEach(file => symbols.add_filename(Buffer.from("posts/" + file + "\0")));

const data = symbols.get_metadata();

console.log(data)
