#include <iostream>

#include "VideoPlayer.h"
#include "Index.h"

VideoPlayer::VideoPlayer(const std::vector<std::string>& folders) {
	_mediaFolders = folders;
}

VideoPlayer::~VideoPlayer() { }

void VideoPlayer::init(const char *const *argv, int argc) {
	_instance = libvlc_new(argc, argv);
	_mediaList = libvlc_media_list_new(_instance);
	_mediaListPlayer = libvlc_media_list_player_new(_instance);
	libvlc_media_list_player_set_media_list(_mediaListPlayer, _mediaList);
}

void VideoPlayer::setCurrentMedia(uint32_t fileId, VideoPlaybackOption option) {
	if (_fileIdToIndex.find(fileId) == _fileIdToIndex.end()) {
		throw std::runtime_error("Media not found: cache does not contain the media requested");
	}

	_currentMedia = fileId;
}

void VideoPlayer::addFileIds(const std::vector<uint32_t>& ids) {
	for (uint32_t fileId : ids) {
		_addFile(fileId);
	}
}

void VideoPlayer::updateMedia(const MediaChangedArgs& args) {
	switch (args.option) {
		case MediaChangedOptions::Added:
			_addFile(args.fileId);
			break;
		case MediaChangedOptions::Modified:
			_updateMediaForFile(args.fileId);
			break;
		case MediaChangedOptions::Removed:
			_removeFile(args.fileId);
			break;
		default:
			// do nothing
			break;
	}
}

uint32_t VideoPlayer::getCurrentMedia() {
	return _currentMedia;
}

void VideoPlayer::playOneShot() {
	if (_mediaListPlayer != nullptr) {
	if (libvlc_media_list_player_is_playing(_mediaListPlayer))
		stop();
	}

	libvlc_media_list_player_set_playback_mode(_mediaListPlayer, libvlc_playback_mode_default);
	libvlc_media_list_player_play_item_at_index(_mediaListPlayer, _fileIdToIndex[_currentMedia]);
}

void VideoPlayer::playLoop() {
	libvlc_media_list_player_set_playback_mode(_mediaListPlayer, libvlc_playback_mode_repeat);
	libvlc_media_list_player_play_item_at_index(_mediaListPlayer, _fileIdToIndex[_currentMedia]);
}

void VideoPlayer::stop() {
	libvlc_media_list_player_stop(_mediaListPlayer);
}

void VideoPlayer::pause() {
	libvlc_media_list_player_pause(_mediaListPlayer);
}

const std::vector<std::string>& VideoPlayer::getMediaFolders() {
	return _mediaFolders;
}

void VideoPlayer::_addFile(uint32_t fileId) {
	if (_fileIdToIndex.find(fileId) == _fileIdToIndex.end()) {
		int32_t i = 0;

		// reuse empty indices in media list if any are available
		if (_emptyIndices.size() > 0) {
			i = _emptyIndices.front();
			_emptyIndices.pop();
		} else {
			i = _mediaListSize;
			_mediaListSize++;
		}

		_fileIdToIndex[fileId] = i;
		_createAndInsertMedia(fileId, i);
	}
}

void VideoPlayer::_updateMediaForFile(uint32_t fileId) {
	if (_fileIdToIndex.find(fileId) != _fileIdToIndex.end()) {
		int32_t i = _fileIdToIndex[fileId];
		_removeMediaAtIndex(i);
		_createAndInsertMedia(fileId, i);
	}
	else {
		_addFile(fileId);
	}
}

void VideoPlayer::_removeFile(uint32_t fileId) {
	if (_fileIdToIndex.find(fileId) != _fileIdToIndex.end()) {
		if (_currentMedia == fileId) {
			stop();
		}

		int32_t i = _fileIdToIndex[fileId];
		_removeMediaAtIndex(i);
		_emptyIndices.emplace(i);
	}
}

void VideoPlayer::_createAndInsertMedia(uint32_t fileId, int32_t i) {
	libvlc_media_t* media = libvlc_media_new_path(_instance, Index::instance().getSystemPath(fileId).c_str());

	libvlc_media_list_lock(_mediaList);
	int32_t ret = libvlc_media_list_insert_media(_mediaList, media, i);
	libvlc_media_list_unlock(_mediaList);

	if (ret == -1)
		throw std::runtime_error("Error: Unable to insert media.  Media list is read only");
}

void VideoPlayer::_removeMediaAtIndex(int32_t i) {
	libvlc_media_list_lock(_mediaList);
	libvlc_media_list_remove_index(_mediaList, i);
	libvlc_media_list_unlock(_mediaList);
}